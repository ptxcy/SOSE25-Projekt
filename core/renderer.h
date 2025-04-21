#ifndef CORE_RENDERER_HEADER
#define CORE_RENDERER_HEADER


#include "buffer.h"
#include "shader.h"


constexpr f32 RENDERER_POSITIONAL_DELETION_CODE = -1247.f;
constexpr u16 RENDERER_MAXIMUM_STEXTURE_COUNT = 512;
constexpr u16 RENDERER_MAXIMUM_SPRITE_COUNT = 512;


struct Sprite
{
	vec2 offset = vec2(0,0);
	vec2 scale = vec2(0,0);
	f32 rotation = .0f;
	f32 alpha = 1.f;
	vec2 tex_position;
	vec2 tex_dimension;
};


class Renderer
{
public:
	Renderer();

	void update();
	void exit();

	// sprite
	PixelBufferComponent* register_sprite_texture(const char* path);
	Sprite* register_sprite(PixelBufferComponent* texture,vec2 position,vec2 size,f32 rotation=.0f,f32 alpha=1.f);
	void assign_sprite_texture(Sprite* sprite,PixelBufferComponent* texture);
	static void delete_sprite_texture(PixelBufferComponent* texture);
	static void delete_sprite(Sprite* sprite);

private:
	void _gpu_upload();
	void _update_sprites();

	// background procedures
	template<typename T> static void _collector(T* xs,std::queue<u16>* os,u16* range,ThreadSignal* signal);

private:

	// ----------------------------------------------------------------------------------------------------
	// Data Management & Pipelines
	VertexArray m_SpriteVertexArray;

	VertexBuffer m_SpriteVertexBuffer;
	VertexBuffer m_SpriteInstanceBuffer;

	ShaderPipeline m_SpritePipeline;

	// ----------------------------------------------------------------------------------------------------
	// Render Object Information

	// textures
	u16 m_SpriteTextureRange = 0;
	GPUPixelBuffer m_GPUSpriteTextures;
	PixelBufferComponent m_SpriteTextures[RENDERER_MAXIMUM_STEXTURE_COUNT];
	std::queue<u16> m_SpriteTextureOverwrite;
	std::queue<TextureData> m_SpriteLoadRequests;

	// sprites
	u16 m_ActiveRange = 0;
	Sprite m_Sprites[RENDERER_MAXIMUM_SPRITE_COUNT];
	std::queue<u16> m_SpriteOverwrite;
};

inline Renderer g_Renderer = Renderer();


#endif
