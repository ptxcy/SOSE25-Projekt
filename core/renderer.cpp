#include "renderer.h"


// ----------------------------------------------------------------------------------------------------
// Background Process Signals

// sprite collector signals
std::mutex mutex_sprite;
std::condition_variable cv_sprite;
bool collect_sprites = false;
void _signal_sprite_collector()
{
	{
		std::lock_guard<std::mutex> lock(mutex_sprite);
		collect_sprites = true;
	}
	cv_sprite.notify_one();
}


// ----------------------------------------------------------------------------------------------------
// Renderer Main Features

/**
 *	setup renderer
 */
Renderer::Renderer()
{
	COMM_MSG(LOG_CYAN,"starting render system");

	COMM_LOG("pre-loading basic geometry data");
	f32 __QuadVertices[] = {
		-.5f,.5f,.0f,.0f, .5f,-.5f,1.f,1.f, .5f,.5f,1.f,.0f,
		.5f,-.5f,1.f,1.f, -.5f,.5f,.0f,.0f, -.5f,-.5f,.0f,1.f
	};

	COMM_LOG("compiling shaders");
	Shader __SpriteVertexShader = Shader("core/shader/sprite.vert",GL_VERTEX_SHADER);
	Shader __DirectFragmentShader = Shader("core/shader/sprite.frag",GL_FRAGMENT_SHADER);

	// ----------------------------------------------------------------------------------------------------
	// Sprite Pipeline
	COMM_LOG("assembling pipelines:");
	COMM_LOG("sprite pipeline");
	m_SpritePipeline.assemble(__SpriteVertexShader,__DirectFragmentShader,4,6,"sprite");
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

	m_SpritePipeline.upload("tex",0);
	m_SpritePipeline.upload_coordinate_system();

	// testing remoev later
	PixelBufferComponent __TrashStateRemoveLater;
	TextureData __TextureData0 = TextureData("./res/test.png");
	TextureData __TextureData1 = TextureData("./res/kek.png");
	TextureData __TextureData2 = TextureData("./res/maps.png");
	TextureData __TextureData3 = TextureData("./res/kid.png");
	__TextureData0.load();
	__TextureData1.load();
	__TextureData2.load();
	__TextureData3.load();
	m_SpriteTextures.atlas.bind();
	m_SpriteTextures.allocate(1500,1500,GL_RGBA);
	m_SpriteTextures.write(&__TrashStateRemoveLater,&__TextureData3);
	m_SpriteTextures.write(&__TrashStateRemoveLater,&__TextureData1);
	m_SpriteTextures.write(&__TrashStateRemoveLater,&__TextureData0);
	m_SpriteTextures.write(&__TrashStateRemoveLater,&__TextureData2);
	Texture::set_texture_parameter_linear_mipmap();
	Texture::set_texture_parameter_clamp_to_edge();
	Texture::generate_mipmap();
	// NOTE when doing this after the detach the subprocess is not longer working, which makes no sense
	// stop of testing block

	// ----------------------------------------------------------------------------------------------------
	// End Pipelines, Start Subprocesses
	std::thread __SpriteCollector(Renderer::_sprite_collector,
								  m_Sprites,&m_SpriteOverwrite,&m_ActiveRange,&m_HelpersActive);
	__SpriteCollector.detach();
	// TODO unsafe, but it will probably work 99% of the time in normal use cases. still improve safety

	COMM_SCC("render system ready.");
}

/**
 *	render visual result
 */
void Renderer::update()
{
	glEnable(GL_BLEND);
	m_SpriteTextures.atlas.bind();
	_update_sprites();
}

/**
 *	exit renderer and end all it's subprocesses
 */
void Renderer::exit()
{
	m_HelpersActive = false;
	_signal_sprite_collector();
}

/**
 *	register a new sprite instance for rendering
 *	\param position: 2-dimensional position of sprite on screen, bounds defined by coordinate system
 *	\param size: width and height of the sprite
 *	\param rotation: (default .0f) rotation of the sprite in degrees
 *	\param alpha: (default 1.f) transparency of sprite clamped between 0 and 1. 0 = invisible -> 1 = opaque
 *	\returns pointer to sprite data for modification purposes
 */
Sprite* Renderer::register_sprite(vec2 position,vec2 size,f32 rotation,f32 alpha)
{
	// determine memory location, overwrite has priority over appending
	u16 i;
	if (m_SpriteOverwrite.size())
	{
		i = m_SpriteOverwrite.front();
		m_SpriteOverwrite.pop();
	}
	else i = m_ActiveRange++;
	COMM_LOG("sprite register at: i: %d -> (%f,%f), %fx%f, %f°",i,position.x,position.y,size.x,size.y,rotation);
	COMM_ERR_COND(i>=RENDERER_MAXIMUM_SPRITE_COUNT,
				  "sprite registration violates maximum range, consider adjusting the respective constant");

	// write information to memory
	m_Sprites[i] = {
		.offset = position,
		.scale = size,
		.rotation = rotation,
		.alpha = alpha
	};
	return &m_Sprites[i];
}

/**
 *	remove sprite from render list. quickly moved out of viewport in main thread, later collected automatically
 *	\param sprite: reference to sprite, being removed
 */
void Renderer::delete_sprite(Sprite* sprite)
{
	// signal sprite removal
	sprite->offset.x = RENDERER_POSITIONAL_DELETION_CODE;
	sprite->scale = vec2(0,0);
	sprite = nullptr;
	_signal_sprite_collector();
}

/**
 *	helper to unclutter the update to all sprites
 */
void Renderer::_update_sprites()
{
	m_SpriteVertexArray.bind();
	m_SpritePipeline.enable();
	m_SpriteInstanceBuffer.bind();
	m_SpriteInstanceBuffer.upload_vertices(m_Sprites,RENDERER_MAXIMUM_SPRITE_COUNT,GL_DYNAMIC_DRAW);
	glDrawArraysInstanced(GL_TRIANGLES,0,6,m_ActiveRange);
}
// TODO dynamic vs static reloading with/without flags (and is it even necessary)
// TODO profile consistent upload expense


// ----------------------------------------------------------------------------------------------------
// Background Processes

/**
 *	automatically collecting deleted sprites and assign memory space for override
 *	\param sprites: storage array for registered arrays
 *	\param overwrites: pointer to queue, listing the memory indices to overwrite in future registrations
 *	\param range: pointer to active range towards furthest address of registered sprites
 *	\param active: reference to background process active flag
 */
void Renderer::_sprite_collector(Sprite* sprites,std::queue<u16>* overwrites,u16* range,volatile bool* active)
{
	COMM_SCC("started sprite collector background process");

	// main loop
	while (*active)
	{
		std::unique_lock<std::mutex> lock(mutex_sprite);
		cv_sprite.wait(lock,[]{ return collect_sprites; });
		collect_sprites = false;
		if (!*active) break;
		COMM_LOG("sprite collector is searching for removed objects...");

		// iterate active sprite memory
		(*overwrites) = std::queue<u16>();
		u16 __Streak = 0;
		for (int i=0;i<(*range);i++)
		{
			// sprite marked to be removed
			if (sprites[i].offset.x==RENDERER_POSITIONAL_DELETION_CODE)
			{
				COMM_MSG(LOG_PURPLE,"marked sprite found at memory index %d and scheduled to overwrite",i);
				__Streak++;
				if (i==(*range)-1) (*range) -= __Streak;
				else overwrites->push(i);
			}

			// end removal streak
			else __Streak = 0;
		}
	}

	COMM_LOG("sprite collector background process finished");
}
