#include "renderer.h"


// ----------------------------------------------------------------------------------------------------
// Background Process Signals

// texture collector signals
ThreadSignal _sprite_texture_signal
#ifdef DEBUG
= { .name = "sprite texture" }
#endif
;

// sprite collector signals
ThreadSignal _sprite_signal
#ifdef DEBUG
= { .name = "sprite" }
#endif
;


// ----------------------------------------------------------------------------------------------------
// Text Component

/**
 *	dynamically align text content based on content dimensions
 */
void Text::align()
{
	offset = position;

	// adjust vertical alignment
	u8 vertical_alignment = 2-(alignment%3);
	offset.y += vertical_alignment*(MATH_CENTER_Y-(font->size*scale*.5));

	// adjust horizontal alignment
	u8 horizontal_alignment = alignment/3;
	if (!!horizontal_alignment)
	{
		f32 wordlen = 0;
		for (char c : data) wordlen += font->glyphs[c-32].advance*scale;
		offset.x += horizontal_alignment*(MATH_CENTER_X-wordlen*.5f);
	}
}

/**
 *	load instance buffer for text content according to specified font
 */
void Text::load_buffer()
{
	bool reallocate = buffer.size()<data.size();
	COMM_LOG_COND(reallocate,"allocating memory for text buffer");
	if (reallocate) buffer = std::vector<TextCharacter>(data.size());

	// load font information for characters
	vec2 __Cursor = offset;
	for (u32 i=0;i<data.size();i++)
	{
		TextCharacter& p_Character = buffer[i];
		PixelBufferComponent& p_Component = font->tex[data[i]-32];
		Glyph& p_Glyph = font->glyphs[data[i]-32];

		// load text data
		p_Character = {
			.offset = __Cursor,
			.scale = p_Glyph.scale*scale,
			.bearing = p_Glyph.bearing*scale,
			.colour = colour,
			.comp = p_Component
		};

		__Cursor.x += p_Glyph.advance*scale;
	}
}


// ----------------------------------------------------------------------------------------------------
// Renderer Main Features

/**
 *	setup renderer
 */
Renderer::Renderer()
{
	COMM_MSG(LOG_CYAN,"starting render system");

	COMM_LOG("starting font rasterizer");
	bool _failed = FT_Init_FreeType(&g_FreetypeLibrary);
	COMM_ERR_COND(_failed,"text rasterizer not available");

	COMM_LOG("pre-loading basic geometry data");
	f32 __QuadVertices[] = {
		-.5f,.5f,.0f,.0f, .5f,-.5f,1.f,1.f, .5f,.5f,1.f,.0f,
		.5f,-.5f,1.f,1.f, -.5f,.5f,.0f,.0f, -.5f,-.5f,.0f,1.f
	};

	COMM_LOG("compiling shaders");
	Shader __SpriteVertexShader = Shader("core/shader/sprite.vert",GL_VERTEX_SHADER);
	Shader __DirectFragmentShader = Shader("core/shader/sprite.frag",GL_FRAGMENT_SHADER);
	Shader __TextVertexShader = Shader("core/shader/text.vert",GL_VERTEX_SHADER);
	Shader __TextFragmentShader = Shader("core/shader/text.frag",GL_FRAGMENT_SHADER);

	// ----------------------------------------------------------------------------------------------------
	// Sprite Pipeline

	COMM_LOG("assembling pipelines:");
	COMM_LOG("sprite pipeline");
	m_SpritePipeline.assemble(__SpriteVertexShader,__DirectFragmentShader,4,10,"sprite");
	m_SpriteVertexArray.bind();
	m_SpriteVertexBuffer.bind();
	m_SpriteVertexBuffer.upload_vertices(__QuadVertices,24);

	m_SpritePipeline.enable();
	m_SpritePipeline.define_attribute("position",2);
	m_SpritePipeline.define_attribute("edge_coordinates",2);

	m_SpriteInstanceBuffer.bind();
	m_SpritePipeline.define_index_attribute("offset",2);
	m_SpritePipeline.define_index_attribute("scale",2);
	m_SpritePipeline.define_index_attribute("rotation",1);
	m_SpritePipeline.define_index_attribute("alpha",1);
	m_SpritePipeline.define_index_attribute("tex_position",2);
	m_SpritePipeline.define_index_attribute("tex_dimension",2);

	m_SpritePipeline.upload("tex",0);
	m_SpritePipeline.upload_coordinate_system();

	COMM_LOG("text pipeline");
	m_TextPipeline.assemble(__TextVertexShader,__TextFragmentShader,4,14,"text");
	m_TextVertexArray.bind();
	m_SpriteVertexBuffer.bind();

	m_TextPipeline.enable();
	m_TextPipeline.define_attribute("position",2);
	m_TextPipeline.define_attribute("edge_coordinates",2);

	m_TextInstanceBuffer.bind();
	m_TextPipeline.define_index_attribute("offset",2);
	m_TextPipeline.define_index_attribute("scale",2);
	m_TextPipeline.define_index_attribute("bearing",2);
	m_TextPipeline.define_index_attribute("colour",4);
	m_TextPipeline.define_index_attribute("atlas_position",2);
	m_TextPipeline.define_index_attribute("atlas_dimension",2);

	m_TextPipeline.upload("tex",0);
	m_TextPipeline.upload_coordinate_system();

	// ----------------------------------------------------------------------------------------------------
	// GPU Memory

	COMM_LOG("allocating sprite memory");
	m_GPUSpriteTextures.atlas.bind();
	m_GPUSpriteTextures.allocate(RENDERER_SPRITE_MEMORY_WIDTH,RENDERER_SPRITE_MEMORY_HEIGHT,GL_RGBA);
	Texture::set_texture_parameter_linear_mipmap();
	Texture::set_texture_parameter_clamp_to_edge();

	COMM_LOG("allocating font memory");
	m_GPUFontTextures.atlas.bind();
	m_GPUFontTextures.allocate(RENDERER_FONT_MEMORY_WIDTH,RENDERER_FONT_MEMORY_HEIGHT,GL_RED);
	Texture::set_texture_parameter_linear_mipmap();
	Texture::set_texture_parameter_clamp_to_edge();

	// ----------------------------------------------------------------------------------------------------
	// Start Subprocesses

	COMM_LOG("starting renderer subprocesses");
	std::thread __SpriteCollector(Renderer::_collector<Sprite>,&m_Sprites,&_sprite_signal);
	__SpriteCollector.detach();
	std::thread __SpriteTextureCollector(Renderer::_collector<PixelBufferComponent>,
										 &m_GPUSpriteTextures.textures,&_sprite_texture_signal);
	__SpriteTextureCollector.detach();

	COMM_SCC("render system ready.");
}

/**
 *	render visual result
 */
void Renderer::update()
{
	PROF_STA(m_ProfilerFullFrame);
	_update_sprites();
	_update_text();
	_gpu_upload();
	PROF_STP(m_ProfilerFullFrame);
}

/**
 *	exit renderer and end all it's subprocesses
 */
void Renderer::exit()
{
	_sprite_texture_signal.exit();
	_sprite_signal.exit();
}

/**
 *	register sprite texture to load and move to sprite pixel buffer
 *	\param path: path to texture file
 *	\returns pointer to texture component info to assign to a sprite later
 */
PixelBufferComponent* Renderer::register_sprite_texture(const char* path)
{
	PixelBufferComponent* p_Comp = m_GPUSpriteTextures.textures.next_free();

	COMM_LOG("sprite texture register of %s",path);
	std::thread __LoadThread(GPUPixelBuffer::load_texture,&m_GPUSpriteTextures,p_Comp,path);
	__LoadThread.detach();

	return p_Comp;
}

/**
 *	register a new sprite instance for rendering
 *	\param texture: sprite texture to be assigned to the sprite canvas
 *	\param position: 2-dimensional position of sprite on screen, bounds defined by coordinate system
 *	\param size: width and height of the sprite
 *	\param rotation: (default .0f) rotation of the sprite in degrees
 *	\param alpha: (default 1.f) transparency of sprite clamped between 0 and 1. 0 = invisible -> 1 = opaque
 *	\returns pointer to sprite data for modification purposes
 */
Sprite* Renderer::register_sprite(PixelBufferComponent* texture,vec2 position,vec2 size,f32 rotation,f32 alpha)
{
	// determine memory location, overwrite has priority over appending
	Sprite* p_Sprite = m_Sprites.next_free();
	COMM_LOG("sprite register at: (%f,%f), %fx%f, %f° -> count = %d",
			 position.x,position.y,size.x,size.y,rotation,m_Sprites.active_range);

	// write information to memory
	(*p_Sprite) = {
		.offset = position,
		.scale = size,
		.rotation = rotation,
		.alpha = alpha,
	};
	Renderer::assign_sprite_texture(p_Sprite,texture);
	return p_Sprite;
}

/**
 *	assign a sprite texture to a sprite canvas
 *	\param sprite: pointer to the sprite canvas received at creation
 *	\param texture: pointer to texture component info received at load request
 */
void Renderer::assign_sprite_texture(Sprite* sprite,PixelBufferComponent* texture)
{
	sprite->tex_position = texture->offset;
	sprite->tex_dimension = texture->dimensions;
}

/**
 *	remove given sprite texture and free memory in array as well as releasing memory space on atlas
 *	\param texture: pointer to texture, which shall be removed
 */
void Renderer::delete_sprite_texture(PixelBufferComponent* texture)
{
	// signal cleanup
	texture->offset.x = RENDERER_POSITIONAL_DELETION_CODE;
	texture = nullptr;
	_sprite_texture_signal.proceed();

	// free texture atlas memory
	m_GPUSpriteTextures.mutex_memory_segments.lock();
	m_GPUSpriteTextures.memory_segments.push_back(*texture);
	m_GPUSpriteTextures.mutex_memory_segments.unlock();
	// TODO merge segments after adding free section to reduce segmentation
}

/**
 *	remove sprite from render list. quickly scaled invisible in main thread, later collected automatically
 *	\param sprite: reference to sprite, being removed
 */
void Renderer::delete_sprite(Sprite* sprite)
{
	sprite->offset.x = RENDERER_POSITIONAL_DELETION_CODE;
	sprite->scale = vec2(0,0);
	sprite = nullptr;
	_sprite_signal.proceed();
}

/**
 *	rasterize a vector font and upload pixel buffer to gpu memory
 *	\param font: font data memory, to use later when writing text with or in style of it
 *	\param path: path to .ttf vector font file
 *	\param size: rasterization size
 */
void Renderer::register_font(Font* font,const char* path,u16 size)
{
	COMM_LOG("font register from source %s",path);
	std::thread __LoadThread(GPUPixelBuffer::load_font,&m_GPUFontTextures,font,path,size);
	__LoadThread.detach();
}

/**
 *	TODO
 */
u16 Renderer::write_text(Font* font,string data,vec2 position,f32 scale,vec4 colour,ScreenAlignment align)
{
	font->signal.wait();
	m_Texts.push_back({
			.font = font,
			.position = position,
			.scale = (f32)scale/font->size,
			.colour = colour,
			.alignment = align,
			.data = data
		});

	m_Texts.back().align();
	m_Texts.back().load_buffer();
	return m_Texts.size()-1;
}
// FIXME experimental memory management, this should happen ideally over the entirety of the possible text range

// TODO add an option to delete text once no longer needed
// TODO cleanup memory on renderer close for all allocated text memory

/**
 *	helper to unclutter the automatic load callbacks for gpu data
 */
void Renderer::_gpu_upload()
{
	m_GPUSpriteTextures.gpu_upload();
	m_GPUFontTextures.gpu_upload();
}
// TODO stall until next frame when frametime budget is used up to avoid framerate issues

/**
 *	update all registered sprites
 */
void Renderer::_update_sprites()
{
	m_SpriteVertexArray.bind();
	m_GPUSpriteTextures.atlas.bind();
	m_SpritePipeline.enable();
	m_SpriteInstanceBuffer.bind();
	m_SpriteInstanceBuffer.upload_vertices(m_Sprites.mem,BUFFER_MAXIMUM_TEXTURE_COUNT,GL_DYNAMIC_DRAW);
	glDrawArraysInstanced(GL_TRIANGLES,0,6,m_Sprites.active_range);
}

/**
 *	update all registered text
 */
void Renderer::_update_text()
{
	// prepare gpu
	m_TextVertexArray.bind();
	m_TextInstanceBuffer.bind();
	m_TextPipeline.enable();
	m_GPUFontTextures.atlas.bind();

	// iterate text entities
	for (Text& p_Text : m_Texts)
	{
		m_TextInstanceBuffer.upload_vertices(p_Text.buffer,GL_DYNAMIC_DRAW);
		glDrawArraysInstanced(GL_TRIANGLES,0,6,p_Text.buffer.size());
	}
}


// ----------------------------------------------------------------------------------------------------
// Background Processes

/**
 *	automatically collecting deleted sprites and assign memory space for override
 *	\param xs: collectable array structure holding removable sprites conforming to the collection rules:
 *			- remove coding is in offset.x as RENDERER_POSITIONAL_DELETION_CODE
 *			- has to be stored as an InPlaceArray to support overwrite and range system
 *	\param signal: background collector needs an activation signal to know when it is sensible to collect
 */
template<typename T> void Renderer::_collector(InPlaceArray<T>* xs,ThreadSignal* signal)
{
	COMM_SCC("started %s collector background process",signal->name);

	// main loop
	while (signal->running)
	{
		signal->wait();
		signal->active = false;
		if (!signal->running) break;
		COMM_LOG("%s collector is searching for removed objects...",signal->name);

		// iterate active sprite memory
		xs->overwrites = std::queue<u16>();
		u16 __Streak = 0;
		for (int i=0;i<xs->active_range;i++)
		{
			// sprite marked to be removed
			if (xs->mem[i].offset.x==RENDERER_POSITIONAL_DELETION_CODE)
			{
				COMM_MSG(LOG_PURPLE,"marked %s found at memory index %d and scheduled to overwrite",
						 signal->name,i);
				// FIXME i'm not so sure i like the logging here
				__Streak++;
				if (i==xs->active_range-1) xs->active_range -= __Streak;
				else xs->overwrites.push(i);
			}

			// end removal streak
			else __Streak = 0;
		}
	}

	COMM_LOG("%s collector background process finished",signal->name);
}
template void Renderer::_collector<Sprite>(InPlaceArray<Sprite>*,ThreadSignal*);
template void Renderer::_collector<PixelBufferComponent>(InPlaceArray<PixelBufferComponent>*,ThreadSignal*);
