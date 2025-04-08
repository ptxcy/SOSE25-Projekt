#include "renderer.h"


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
	Shader __DirectFragmentShader = Shader("core/shader/direct.frag",GL_FRAGMENT_SHADER);

	// ----------------------------------------------------------------------------------------------------
	// Sprite Pipeline
	COMM_LOG("assembling pipelines:");
	COMM_LOG("sprite pipeline");
	m_SpritePipeline.assemble(__SpriteVertexShader,__DirectFragmentShader,4,5,"sprite");
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

	m_SpritePipeline.upload("tex",0);
	m_SpritePipeline.upload_coordinate_system();

	// ----------------------------------------------------------------------------------------------------
	// End Pipelines
	COMM_SCC("render system ready.");
}

/**
 *	render visual result
 */
void Renderer::update()
{
	_update_sprites();
}

/**
 *	register a new sprite instance for rendering
 *	\param position: 2-dimensional position of sprite on screen, bounds defined by coordinate system
 *	\param size: width and height of the sprite
 *	\param rotation: (default .0f) rotation of the sprite in degrees
 *	\returns pointer to sprite data for modification purposes
 */
Sprite* Renderer::register_sprite(vec2 position,vec2 size,f32 rotation)
{
	COMM_LOG("sprite register at: (%f,%f), %fx%f, %f°",position.x,position.y,size.x,size.y,rotation);
	COMM_ERR_COND(RENDERER_MAXIMUM_SPRITE_COUNT<=m_ActiveRange+1,
				  "sprite registration violates maximum range, consider adjusting the respective constant");

	u16 i = m_ActiveRange++;
	m_Sprites[i] = {
		.offset = position,
		.scale = size,
		.rotation = rotation
	};
	return &m_Sprites[i];
};

/**
 *	remove sprite from render list. quickly moved out of viewport in main thread, later collected automatically
 *	\param sprite: reference to sprite, being removed
 */
void Renderer::delete_sprite(Sprite* sprite)
{
	sprite->offset.x = RENDERER_POSITIONAL_DELETION_CODE;
}
// TODO allow to overwrite deleted sprites with next registration
//		background checker for positional codes (scale and alpha does not make sense for animation reasons)
//		then after deletion code setting set a flag to search for removable sprites
//		a thread will then be unleashed while this flag is active, starting with setting the flag inactive
//		then the thread proceeds with removal, should another removal take place during the search
//		the flag will be active again and the loop wont be exited. abusing race conditions to our advantage.

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
