#ifndef CORE_RENDERER_HEADER
#define CORE_RENDERER_HEADER


#include "buffer.h"
#include "shader.h"


constexpr u16 RENDERER_MAXIMUM_SPRITE_COUNT = 512;


struct Sprite
{
	vec2 offset = vec2(0,0);
	vec2 scale = vec2(0,0);
	f32 rotation = .0f;
};


class Renderer
{
public:
	Renderer();

	void update();

	Sprite* register_sprite(vec2 position,vec2 size,f32 rotation=.0f);

private:
	void _update_sprites();

private:

	// ----------------------------------------------------------------------------------------------------
	// Data Management & Pipelines
	VertexArray m_SpriteVertexArray;

	VertexBuffer m_SpriteVertexBuffer;
	VertexBuffer m_SpriteInstanceBuffer;

	ShaderPipeline m_SpritePipeline;

	// ----------------------------------------------------------------------------------------------------
	// Render Object Information
	u16 m_ActiveRange = 0;
	Sprite m_Sprites[RENDERER_MAXIMUM_SPRITE_COUNT];
};

inline Renderer g_Renderer = Renderer();


#endif
