#ifndef CORE_RENDERER_HEADER
#define CORE_RENDERER_HEADER


#include "buffer.h"
#include "shader.h"


constexpr f32 RENDERER_POSITIONAL_DELETION_CODE = -1247.f;
constexpr u16 RENDERER_MAXIMUM_SPRITE_COUNT = 512;


struct Sprite
{
	vec2 offset = vec2(0,0);
	vec2 scale = vec2(0,0);
	f32 rotation = .0f;
	f32 alpha = 1.f;
};


class Renderer
{
public:
	Renderer();

	void update();
	void exit();

	// sprite
	void register_sprite_texture(const char* path);
	Sprite* register_sprite(vec2 position,vec2 size,f32 rotation=.0f,f32 alpha=1.f);
	static void delete_sprite(Sprite* sprite);

private:
	void _gpu_upload();
	void _update_sprites();

	// background procedures
	static void _sprite_collector(Sprite* sprites,std::queue<u16>* overwrites,u16* range,volatile bool* active);

private:
	volatile bool m_HelpersActive = true;

	// ----------------------------------------------------------------------------------------------------
	// Data Management & Pipelines
	VertexArray m_SpriteVertexArray;

	VertexBuffer m_SpriteVertexBuffer;
	VertexBuffer m_SpriteInstanceBuffer;

	ShaderPipeline m_SpritePipeline;

	// ----------------------------------------------------------------------------------------------------
	// Render Object Information

	// textures
	GPUPixelBuffer m_SpriteTextures;
	std::queue<TextureData> m_SpriteLoadRequests;

	// sprites
	u16 m_ActiveRange = 0;
	Sprite m_Sprites[RENDERER_MAXIMUM_SPRITE_COUNT];
	std::queue<u16> m_SpriteOverwrite = std::queue<u16>();  // FIXME ??why did i do that
};

inline Renderer g_Renderer = Renderer();


#endif
