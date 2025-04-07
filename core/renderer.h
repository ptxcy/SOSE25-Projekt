#ifndef CORE_RENDERER_HEADER
#define CORE_RENDERER_HEADER


#include "buffer.h"
#include "shader.h"


struct Sprite
{
	vec2 offset;
	vec2 scale;
	f32 rotation;
};


class Renderer
{
public:
	Renderer();

	void update();

	void load_sprites();

public:

	// ----------------------------------------------------------------------------------------------------
	// Render Object Information
	std::vector<Sprite> sprites = std::vector<Sprite>();

private:

	// ----------------------------------------------------------------------------------------------------
	// Data Management & Pipelines
	VertexArray m_SpriteVertexArray;

	VertexBuffer m_SpriteVertexBuffer;
	VertexBuffer m_SpriteInstanceBuffer;

	ShaderPipeline m_SpritePipeline;
};

inline Renderer g_Renderer = Renderer();


#endif
