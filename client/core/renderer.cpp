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
	// calculate text dimensions
	f32 wordlen = font->estimate_wordlength(data);
	dimensions = vec2(wordlen,font->size)*scale;

	// calculate position based on alignment and dimensions
	if (alignment.align<SCREEN_ALIGN_NEUTRAL)
	{
		vec2 __AlignedOffset = Renderer::align({ position,dimensions },alignment);
		offset.x = __AlignedOffset.x;
		offset.y = __AlignedOffset.y;
		return;
	}

	// compliment dimensions by offset
	offset.x = position.x-dimensions.x*.5f;
	offset.y = position.y-dimensions.y*.33f;
}

/**
 *	load instance buffer for text content according to specified font
 */
void Text::load_buffer()
{
	bool reallocate = buffer.capacity()<data.size();
	COMM_LOG_COND(reallocate,"allocating memory for text buffer");
	buffer.resize(data.size());

	// load font information for characters
	vec3 __Cursor = vec3(offset.x,offset.y,position.z);
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

/**
 *	calculate horizontal character intersection
 *	\param pos: horizontal intersecting pixel position
 *	\returns 0 if end of the word, buffer size if beginning of the word is last intersection and else in-between
 */
u32 Text::intersection(f32 pos)
{
	u32 i = 0;

	// starting intersection
	if (!data.size()) return 0;
	f32 __Cursor = position.x+font->glyphs[data[0]-32].advance*.5*scale;

	// iterate following characters
	while (i<data.size()-1&&__Cursor<pos)
	{
		__Cursor += (font->glyphs[data[i]-32].advance+font->glyphs[data[i+1]-32].advance)*.5*scale;
		i++;
	}
	return data.size()-i;
}


// ----------------------------------------------------------------------------------------------------
// Mesh Component

/**
 *	load mesh geometry from .obj file
 *	\param path: path to .obj file explicitly defining geometry
 */
Mesh::Mesh(const char* path)
{
	vector<vec3> __Positions;
	vector<vec2> __UVCoordinates;
	vector<vec3> __Normals;
	vector<u32> __PositionIndices;
	vector<u32> __UVIndices;
	vector<u32> __NormalIndices;

	// open source file
	FILE* __OBJFile = fopen(path,"r");
	if (__OBJFile==NULL)
	{
		COMM_ERR("geometry definition file %s could not be found",path);
		return;
	}

	// iterate and sort geometry information
	char __Command[128];
	while (fscanf(__OBJFile,"%s",__Command)!=EOF)
	{
		// process position prefix
		if (!strcmp(__Command,"v"))
		{
			vec3 __Position;
			fscanf(__OBJFile,"%f %f %f\n",&__Position.x,&__Position.y,&__Position.z);
			__Positions.push_back(__Position);
		}

		// process uv coordinate prefix
		else if (!strcmp(__Command,"vt"))
		{
			vec2 __UVCoordinate;
			fscanf(__OBJFile,"%f %f\n",&__UVCoordinate.x,&__UVCoordinate.y);
			__UVCoordinates.push_back(__UVCoordinate);
		}

		// process normal prefix
		else if (!strcmp(__Command,"vn"))
		{
			vec3 __Normal;
			fscanf(__OBJFile,"%f %f %f\n",&__Normal.x,&__Normal.y,&__Normal.z);
			__Normals.push_back(__Normal);
		}

		// process face prefix
		else if (!strcmp(__Command,"f"))
		{
			u32 __PositionIndex[3];
			u32 __UVIndex[3];
			u32 __NormalIndex[3];
			fscanf(
					__OBJFile,"%u/%u/%u %u/%u/%u %u/%u/%u\n",
					&__PositionIndex[0],&__UVIndex[0],&__NormalIndex[0],
					&__PositionIndex[1],&__UVIndex[1],&__NormalIndex[1],
					&__PositionIndex[2],&__UVIndex[2],&__NormalIndex[2]
				);
			for (u8 i=0;i<3;i++)
			{
				__PositionIndices.push_back(__PositionIndex[i]);
				__UVIndices.push_back(__UVIndex[i]);
				__NormalIndices.push_back(__NormalIndex[i]);
			}
		}
	}

	// close file & allocate memory
	fclose(__OBJFile);
	vertices.reserve(__PositionIndices.size());

	// iterate faces & write vertices
	for (u32 i=0;i<__PositionIndices.size();i+=3)
	{
		for (u8 j=0;j<3;j++)
		{
			u32 n = i+j;
			Vertex __Vertex = {
				.position = __Positions[__PositionIndices[n]-1],
				.uv = __UVCoordinates[__UVIndices[n]-1],
				.normal = __Normals[__NormalIndices[n]-1]
			};
			vertices.push_back(__Vertex);
		}

		// precalculate tangent for gram-schmidt reorthogonalization & normal mapping
		vec3 __EdgeDelta0 = vertices[i+1].position-vertices[i].position;
		vec3 __EdgeDelta1 = vertices[i+2].position-vertices[i].position;
		vec2 __UVDelta0 = vertices[i+1].uv-vertices[i].uv;
		vec2 __UVDelta1 = vertices[i+2].uv-vertices[i].uv;
		f32 __Factor = 1.f/(__UVDelta0.x*__UVDelta1.y-__UVDelta0.y*__UVDelta1.x);
		glm::mat2x3 __CombinedEdges = glm::mat2x3(__EdgeDelta0,__EdgeDelta1);
		vec2 __CombinedUVs = vec2(__UVDelta1.y,-__UVDelta0.y);
		vec3 __Tangent = __Factor*(__CombinedEdges*__CombinedUVs);
		__Tangent = glm::normalize(__Tangent);
		for (u8 j=0;j<3;j++) vertices[i+j].tangent = __Tangent;
	}
}

/**
 *	upload load batch geometry to gpu
 */
void GeometryBatch::load()
{
	COMM_LOG("uploading geometry batch to gpu");
	vao.bind();
	vbo.bind();
	vbo.upload_vertices(geometry);
	shader->map(&vbo);
}

/**
 *	load particle mesh into batch memory
 */
void ParticleBatch::load()
{
	COMM_LOG("loading particle mesh geometry information");
	vao.bind();
	vbo.bind();
	vbo.upload_vertices(geometry);
	shader->map(&vbo,&ibo);
}
// FIXME absolutely twinning :3


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
	f32 __CanvasVertices[] = {
		-1.f,1.f,.0f,1.f, 1.f,-1.f,1.f,.0f, 1.f,1.f,1.f,1.f,
		1.f,-1.f,1.f,.0f, -1.f,1.f,.0f,1.f, -1.f,-1.f,.0f,.0f
	};

	COMM_LOG("compiling shaders");
	VertexShader __SpriteVertexShader = VertexShader("core/shader/sprite.vert");
	FragmentShader __DirectFragmentShader = FragmentShader("core/shader/sprite.frag");
	VertexShader __TextVertexShader = VertexShader("core/shader/text.vert");
	FragmentShader __TextFragmentShader = FragmentShader("core/shader/text.frag");
	VertexShader __CanvasVertexShader = VertexShader("core/shader/canvas.vert");
	FragmentShader __CanvasFragmentShader = FragmentShader("core/shader/canvas.frag");

	// ----------------------------------------------------------------------------------------------------
	// Sprite Pipeline

	COMM_LOG("assembling pipelines:");
	COMM_LOG("sprite pipeline");
	m_SpritePipeline.assemble(__SpriteVertexShader,__DirectFragmentShader);
	m_SpriteVertexArray.bind();
	m_SpriteVertexBuffer.bind();
	m_SpriteVertexBuffer.upload_vertices(__QuadVertices,24);
	m_SpritePipeline.map(&m_SpriteVertexBuffer,&m_SpriteInstanceBuffer);

	m_SpritePipeline.upload("tex",RENDERER_TEXTURE_SPRITES);
	m_SpritePipeline.upload_coordinate_system();

	COMM_LOG("text pipeline");
	m_TextPipeline.assemble(__TextVertexShader,__TextFragmentShader);
	m_TextVertexArray.bind();
	m_SpriteVertexBuffer.bind();
	m_TextPipeline.map(&m_SpriteVertexBuffer,&m_TextInstanceBuffer);

	m_TextPipeline.upload("tex",RENDERER_TEXTURE_FONTS);
	m_TextPipeline.upload_coordinate_system();

	COMM_LOG("canvas pipeline");
	m_CanvasPipeline.assemble(__CanvasVertexShader,__CanvasFragmentShader);
	m_CanvasVertexArray.bind();
	m_CanvasVertexBuffer.bind();
	m_CanvasVertexBuffer.upload_vertices(__CanvasVertices,24);
	m_CanvasPipeline.map(&m_CanvasVertexBuffer);

	m_CanvasPipeline.upload("tex",RENDERER_TEXTURE_FORWARD);

	// ----------------------------------------------------------------------------------------------------
	// GPU Memory

	COMM_LOG("allocating sprite memory");
	m_GPUSpriteTextures.atlas.bind(RENDERER_TEXTURE_SPRITES);
	m_GPUSpriteTextures.allocate(RENDERER_SPRITE_MEMORY_WIDTH,RENDERER_SPRITE_MEMORY_HEIGHT,GL_RGBA);
	Texture::set_texture_parameter_linear_mipmap();
	Texture::set_texture_parameter_clamp_to_edge();

	COMM_LOG("allocating font memory");
	m_GPUFontTextures.atlas.bind(RENDERER_TEXTURE_FONTS);
	m_GPUFontTextures.allocate(RENDERER_FONT_MEMORY_WIDTH,RENDERER_FONT_MEMORY_HEIGHT,GL_RED);
	Texture::set_texture_parameter_linear_mipmap();
	Texture::set_texture_parameter_clamp_to_edge();

	// ----------------------------------------------------------------------------------------------------
	// Render Targets

	COMM_LOG("creating scene render target");
	m_ForwardFrameBuffer.start();
	m_ForwardFrameBuffer.define_colour_component(0,FRAME_RESOLUTION_X,FRAME_RESOLUTION_Y);
	m_ForwardFrameBuffer.define_depth_component(FRAME_RESOLUTION_X,FRAME_RESOLUTION_Y);
	m_ForwardFrameBuffer.finalize();
	Framebuffer::stop();

	// ----------------------------------------------------------------------------------------------------
	// Start Subprocesses

	COMM_LOG("starting renderer subprocesses");
	_sprite_signal.stall();
	m_SpriteCollector = thread(Renderer::_collector<Sprite>,&m_Sprites,&_sprite_signal);
	m_SpriteCollector.detach();
	_sprite_texture_signal.stall();
	m_SpriteTextureCollector = thread(Renderer::_collector<PixelBufferComponent>,
									  &m_GPUSpriteTextures.textures,&_sprite_texture_signal);
	m_SpriteTextureCollector.detach();
	COMM_SCC("render system ready.");
}
// TODO join collector processes when exiting renderer, or maybe just let the os handle that and not care?

/**
 *	render visual result
 */
void Renderer::update()
{
	m_FrameStart = std::chrono::steady_clock::now();

	// 3D segment
	m_ForwardFrameBuffer.start();
	_update_mesh();
	Framebuffer::stop();

	// rendertargets
	glDisable(GL_DEPTH_TEST);
	_update_canvas();
	glEnable(GL_DEPTH_TEST);

	// 2D segment
	_update_sprites();
	_update_text();

	// end-frame gpu management
	_gpu_upload();
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
	m_GPUSpriteTextures.signal.stall();

	COMM_LOG("sprite texture register of %s",path);
	thread __LoadThread(GPUPixelBuffer::load_texture,&m_GPUSpriteTextures,p_Comp,path);
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
 *	\param alignment: (default fullscreen neutral) sprite position alignment within borders
 *	\returns pointer to sprite data for modification purposes
 */
Sprite* Renderer::register_sprite(PixelBufferComponent* texture,vec3 position,vec2 size,f32 rotation,
								  f32 alpha,Alignment alignment)
{
	// determine memory location, overwrite has priority over appending
	Sprite* p_Sprite = m_Sprites.next_free();
	COMM_LOG("sprite register at: (%f,%f), %fx%f, %f° -> count = %d",
			 position.x,position.y,size.x,size.y,rotation,m_Sprites.active_range);

	// align sprite into borders
	if (alignment.align!=SCREEN_ALIGN_NEUTRAL)
	{
		vec2 hsize = size*.5f;
		vec2 __AlignedPosition = align({ vec2(position)-hsize,size },alignment)+size;
		position.x = __AlignedPosition.x;
		position.y = __AlignedPosition.y;
	}

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
	m_GPUSpriteTextures.signal.wait();
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
 *	\param path: path to .ttf vector font file
 *	\param size: rasterization size
 *	\returns font data memory, to use later when writing text with or in style of it
 */
Font* Renderer::register_font(const char* path,u16 size)
{
	COMM_LOG("font register from source %s",path);
	Font* p_Font = m_Fonts.next_free();
	m_GPUFontTextures.signal.stall();
	thread __LoadThread(GPUPixelBuffer::load_font,&m_GPUFontTextures,p_Font,path,size);
	__LoadThread.detach();
	return p_Font;
}

/**
 *	write text on screen
 *	\param font: pointer to loaded font
 *	\param data: text content to be displayed in given font
 *	\param position: positional offset of the text based on screen alignment
 *	\param scale: intuitive absolute text scaling in pixels, supported by automatic adaptive resolution
 *	\param colour: (default vec4(1)) text starting colour of all characters
 *	\param align: (default SCREEN_ALIGN_BOTTOMLEFT) text alignment on screen, modified by positional offset
 *	\returns list container of created text
 */
lptr<Text> Renderer::write_text(Font* font,string data,vec3 position,f32 scale,vec4 colour,Alignment align)
{
	m_GPUFontTextures.signal.wait();
	m_Texts.push_back({
			.font = font,
			.position = position,
			.scale = (f32)scale/font->size,
			.colour = colour,
			.alignment = align,
			.data = data
		});

	lptr<Text> p_Text = std::prev(m_Texts.end());
	p_Text->align();
	p_Text->load_buffer();
	return p_Text;
}

/**
 *	register shader pipeline
 *	\param vs: vertex shader
 *	\param fs: fragment shader
 *	returns pointer to registered shader pipeline
 */
lptr<ShaderPipeline> Renderer::register_pipeline(VertexShader& vs,FragmentShader& fs)
{
	m_ShaderPipelines.push_back(ShaderPipeline());
	lptr<ShaderPipeline> p_Pipeline = std::prev(m_ShaderPipelines.end());
	p_Pipeline->assemble(vs,fs);
	return p_Pipeline;
}

/**
 *	register triangle mesh batch
 *	\param pipeline: shader pipeline, handling pixel output for newly created batch
 *	\returns pointer to created triangle mesh batch
 */
lptr<GeometryBatch> Renderer::register_geometry_batch(lptr<ShaderPipeline> pipeline)
{
	m_GeometryBatches.push_back({ .shader = pipeline });
	return std::prev(m_GeometryBatches.end());
}

/**
 *	register particle batch
 *	\param pipeline: shader pipeline, handling pixel output for newly created batch
 *	\returns pointer to created particle batch
 */
lptr<ParticleBatch> Renderer::register_particle_batch(lptr<ShaderPipeline> pipeline)
{
	m_ParticleBatches.push_back({ .shader = pipeline });
	return std::prev(m_ParticleBatches.end());
}

/**
 *	geometry realignment based on position
 *	\param geom: intersection rectangle over aligning geometry
 *	\param alignment: (default fullscreen neutral) target alignment within specified border
 *	\returns new position of geometry after alignment process
 */
vec2 Renderer::align(Rect geom,Alignment alignment)
{
	// setup
	vec2 __Position = geom.position;
	vec2 __GeomCenter = geom.extent*vec2(.5f);
	vec2 __BorderCenter = alignment.border.extent*vec2(.5f)+alignment.border.position;

	// adjust vertical alignment
	u8 vertical_alignment = 2-(alignment.align%3);
	__Position.y += vertical_alignment*(__BorderCenter.y-__GeomCenter.y);

	// adjust horizontal alignment
	u8 horizontal_alignment = alignment.align/3;
	if (!!horizontal_alignment) __Position.x += horizontal_alignment*(__BorderCenter.x-__GeomCenter.x);
	return __Position;
}

/**
 *	helper to unclutter the automatic load callbacks for gpu data
 */
void Renderer::_gpu_upload()
{
	m_GPUSpriteTextures.gpu_upload(RENDERER_TEXTURE_SPRITES,m_FrameStart);
	m_GPUFontTextures.gpu_upload(RENDERER_TEXTURE_FONTS,m_FrameStart);
}

/**
 *	update all registered sprites
 */
void Renderer::_update_sprites()
{
	m_SpriteVertexArray.bind();
	m_SpriteInstanceBuffer.bind();
	m_SpriteInstanceBuffer.upload_vertices(m_Sprites.mem,BUFFER_MAXIMUM_TEXTURE_COUNT,GL_DYNAMIC_DRAW);
	m_SpritePipeline.enable();
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

	// iterate text entities
	for (Text& p_Text : m_Texts)
	{
		m_TextInstanceBuffer.upload_vertices(p_Text.buffer,GL_DYNAMIC_DRAW);
		glDrawArraysInstanced(GL_TRIANGLES,0,6,p_Text.buffer.size());
	}
}

/**
 *	update framebuffer representations
 */
void Renderer::_update_canvas()
{
	m_CanvasVertexArray.bind();
	m_CanvasPipeline.enable();
	m_ForwardFrameBuffer.bind_colour_component(RENDERER_TEXTURE_FORWARD,0);
	glDrawArrays(GL_TRIANGLES,0,6);
}

/**
 *	update triangle meshes
 */
void Renderer::_update_mesh()
{
	// iterate static geometry
	for (GeometryBatch& p_Batch : m_GeometryBatches)
	{
		p_Batch.shader->enable();
		p_Batch.vao.bind();
		for (u8 i=0;i<p_Batch.textures.size();i++) p_Batch.textures[i].bind(RENDERER_TEXTURE_UNMAPPED+i);
		glDrawArrays(GL_TRIANGLES,0,p_Batch.vertex_count);
	}

	// iterate particle geometry
	for (ParticleBatch& p_Batch : m_ParticleBatches)
	{
		p_Batch.shader->enable();
		p_Batch.vao.bind();
		glDrawArraysInstanced(GL_TRIANGLES,0,p_Batch.vertex_count,p_Batch.active_particles);
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
		signal->stall();
		if (!signal->running) break;
		COMM_LOG("%s collector is searching for removed objects...",signal->name);

		// iterate active sprite memory
		xs->overwrites = queue<u16>();
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
