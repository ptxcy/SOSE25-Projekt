#include "renderer.h"


// ----------------------------------------------------------------------------------------------------
// Background Process Signals

// texture upload locking
std::mutex _mutex_sprite_requests;

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

	// ----------------------------------------------------------------------------------------------------
	// GPU Memory

	// allocate sprite memory
	m_GPUSpriteTextures.atlas.bind();
	m_GPUSpriteTextures.allocate(1500,1500,GL_RGBA);
	Texture::set_texture_parameter_linear_mipmap();
	Texture::set_texture_parameter_clamp_to_edge();

	// ----------------------------------------------------------------------------------------------------
	// Start Subprocesses
	std::thread __SpriteCollector(Renderer::_collector<Sprite>,&m_Sprites,&_sprite_signal);
	__SpriteCollector.detach();
	std::thread __SpriteTextureCollector(Renderer::_collector<PixelBufferComponent>,
										 &m_SpriteTextures,&_sprite_texture_signal);
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
 *	\returns pointer to texture component info to assign the texture to a sprite later
 */
PixelBufferComponent* Renderer::register_sprite_texture(const char* path)
{
	PixelBufferComponent* p_Comp = m_SpriteTextures.next_free();
	COMM_LOG("sprite texture register of %s",path);

	std::thread __LoadThread(GPUPixelBuffer::load,
							 &m_GPUSpriteTextures,&m_SpriteLoadRequests,p_Comp,&_mutex_sprite_requests,path);
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
 *	helper to unclutter the automatic load callbacks for gpu data
 */
void Renderer::_gpu_upload()
{
	m_GPUSpriteTextures.atlas.bind();
	_mutex_sprite_requests.lock();
	while (m_SpriteLoadRequests.size())
	{
		TextureData& __Data = m_SpriteLoadRequests.front();

		COMM_AWT("uploading sprite texture buffer at %d,%d to gpu",__Data.x,__Data.y);
		__Data.gpu_upload(__Data.x,__Data.y);
		m_SpriteLoadRequests.pop();
		COMM_CNF();
	}
	_mutex_sprite_requests.unlock();
	Texture::generate_mipmap();
}
// FIXME performance will suffer when generating mipmap every time the loop condition breaks
// TODO stall until next frame when frametime budget is used up to avoid framerate issues

/**
 *	helper to unclutter the update to all sprites
 */
void Renderer::_update_sprites()
{
	m_SpriteVertexArray.bind();
	m_GPUSpriteTextures.atlas.bind();
	m_SpritePipeline.enable();
	m_SpriteInstanceBuffer.bind();
	m_SpriteInstanceBuffer.upload_vertices(m_Sprites.mem,RENDERER_MAXIMUM_SPRITE_COUNT,GL_DYNAMIC_DRAW);
	glDrawArraysInstanced(GL_TRIANGLES,0,6,m_Sprites.active_range);
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
