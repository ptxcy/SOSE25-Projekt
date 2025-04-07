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
	m_SpritePipeline.assemble(__SpriteVertexShader,__DirectFragmentShader,4,0,"sprite");
	m_SpriteVertexArray.bind();
	m_SpriteVertexBuffer.bind();
	m_SpriteVertexBuffer.upload_vertices(__QuadVertices,24);

	m_SpritePipeline.enable();
	m_SpritePipeline.define_attribute("position",2);
	m_SpritePipeline.define_attribute("edge_coordinates",2);

	m_SpritePipeline.upload("texture",0);

	// ----------------------------------------------------------------------------------------------------
	// End Pipelines
	COMM_SCC("render system ready.");
}

/**
 *	render visual result
 */
void Renderer::update()
{
	m_SpriteVertexArray.bind();
	m_SpritePipeline.enable();
	glDrawArrays(GL_TRIANGLES,0,6);
}
