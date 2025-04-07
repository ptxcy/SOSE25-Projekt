#ifndef CORE_RENDERER_HEADER
#define CORE_RENDERER_HEADER


#include "buffer.h"
#include "shader.h"


class Renderer
{
public:
	Renderer();

	void update();

private:
	VertexArray m_SpriteVertexArray;

	VertexBuffer m_SpriteVertexBuffer;

	ShaderPipeline m_SpritePipeline;
};

inline Renderer g_Renderer = Renderer();


#endif
