#ifndef CORE_RENDERER_HEADER
#define CORE_RENDERER_HEADER


#include "buffer.h"
#include "shader.h"


constexpr f32 RENDERER_POSITIONAL_DELETION_CODE = -1247.f;

enum TextureChannelMap : u16
{
	RENDERER_TEXTURE_SPRITES,
	RENDERER_TEXTURE_FONTS,
	RENDERER_TEXTURE_FORWARD,
	RENDERER_TEXTURE_UNMAPPED
};


// ----------------------------------------------------------------------------------------------------
// States

enum ScreenAlignment
{
	SCREEN_ALIGN_TOPLEFT,
	SCREEN_ALIGN_CENTERLEFT,
	SCREEN_ALIGN_BOTTOMLEFT,
	SCREEN_ALIGN_TOPCENTER,
	SCREEN_ALIGN_CENTER,
	SCREEN_ALIGN_BOTTOMCENTER,
	SCREEN_ALIGN_TOPRIGHT,
	SCREEN_ALIGN_CENTERRIGHT,
	SCREEN_ALIGN_BOTTOMRIGHT,
	SCREEN_ALIGN_NEUTRAL,
};

struct Alignment
{
	Rect border = { vec2(0),vec2(MATH_CARTESIAN_XRANGE,MATH_CARTESIAN_YRANGE) };
	ScreenAlignment align = SCREEN_ALIGN_NEUTRAL;
};


// ----------------------------------------------------------------------------------------------------
// GPU Data Structures

struct Sprite
{
	vec3 offset = vec3(0);
	vec2 scale = vec2(0);
	f32 rotation = .0f;
	f32 alpha = 1.f;
	vec2 tex_position;
	vec2 tex_dimension;
};

struct TextCharacter
{
	vec3 offset = vec3(0);
	vec2 scale = vec2(0);
	vec2 bearing = vec2(0);
	vec4 colour = vec4(1);
	PixelBufferComponent comp;
};

struct Vertex
{
	vec3 position;
	vec2 uv;
	vec3 normal;
	vec3 tangent;
};


// ----------------------------------------------------------------------------------------------------
// Entity Data

struct Text
{
	// utility
	void align();
	void load_buffer();
	u32 intersection(f32 pos);

	// data
	Font* font;
	vec3 position;
	vec2 offset;
	f32 scale;
	vec2 dimensions;
	vec4 colour;
	Alignment alignment;
	string data;
	vector<TextCharacter> buffer;
};

class Mesh
{
public:
	Mesh(const char* path);

public:
	vector<Vertex> vertices;
};


// ----------------------------------------------------------------------------------------------------
// Batches

struct GeometryTuple
{
	size_t offset;
	size_t vertex_count;
	vector<Texture*> textures;
};

struct GeometryBatch
{
	// utility
	void add_geometry(Mesh& mesh,vector<Texture*>& tex);
	void add_geometry(void* verts,size_t vsize,size_t ssize,vector<Texture*>& tex);
	void load(vector<string> texvars);

	// data
	VertexArray vao;
	VertexBuffer vbo;
	lptr<ShaderPipeline> shader;
	vector<GeometryTuple> object;
	vector<float> geometry;
	u32 geometry_cursor = 0;
};
// TODO detached texture load after definition
// TODO also link to registered textures and iterate to reduce memory consumption

struct ParticleBatch
{
	// utility
	void load(Mesh& mesh,u32 particles);
	void load(void* verts,size_t vsize,size_t ssize,u32 particles);

	// data
	VertexArray vao;
	VertexBuffer vbo;
	VertexBuffer ibo;
	lptr<ShaderPipeline> shader;
	vector<float> geometry;
	u32 vertex_count;
	u32 active_particles = 0;
};


// ----------------------------------------------------------------------------------------------------
// Renderer Component

class Renderer
{
public:
	Renderer();

	void update();
	void exit();

	// sprite
	PixelBufferComponent* register_sprite_texture(const char* path);
	Sprite* register_sprite(PixelBufferComponent* texture,vec3 position,vec2 size,f32 rotation=.0f,
							f32 alpha=1.f,Alignment alignment={});
	void assign_sprite_texture(Sprite* sprite,PixelBufferComponent* texture);
	void delete_sprite_texture(PixelBufferComponent* texture);
	static void delete_sprite(Sprite* sprite);

	// text
	Font* register_font(const char* path,u16 size);
	lptr<Text> write_text(Font* font,string data,vec3 position,f32 scale,vec4 colour=vec4(1),Alignment align={});
	inline void delete_text(lptr<Text> text) { m_Texts.erase(text); }

	// textures
	Texture* register_texture(const char* path);

	// scene
	lptr<ShaderPipeline> register_pipeline(VertexShader& vs,FragmentShader& fs);
	lptr<GeometryBatch> register_geometry_batch(lptr<ShaderPipeline> pipeline);
	lptr<ParticleBatch> register_particle_batch(lptr<ShaderPipeline> pipeline);

	// utility
	static vec2 align(Rect geom,Alignment alignment);

private:
	void _gpu_upload();
	void _update_sprites();
	void _update_text();
	void _update_canvas();
	void _update_mesh();

	// background procedures
	template<typename T> static void _collector(InPlaceArray<T>* xs,ThreadSignal* signal);

private:

	// ----------------------------------------------------------------------------------------------------
	// Runtime Profiler
#ifdef DEBUG
	RuntimeProfilerData m_ProfilerFullFrame = PROF_CRT("full frametime");
#endif
	std::chrono::steady_clock::time_point m_FrameStart;

	// ----------------------------------------------------------------------------------------------------
	// Threading

	thread m_SpriteCollector;
	thread m_SpriteTextureCollector;

	// ----------------------------------------------------------------------------------------------------
	// Data Management & Pipelines

	VertexArray m_SpriteVertexArray;
	VertexArray m_TextVertexArray;
	VertexArray m_CanvasVertexArray;

	VertexBuffer m_SpriteVertexBuffer;
	VertexBuffer m_CanvasVertexBuffer;

	VertexBuffer m_SpriteInstanceBuffer;
	VertexBuffer m_TextInstanceBuffer;

	ShaderPipeline m_SpritePipeline;
	ShaderPipeline m_TextPipeline;
	ShaderPipeline m_CanvasPipeline;

	Framebuffer m_ForwardFrameBuffer = Framebuffer(1);

	// ----------------------------------------------------------------------------------------------------
	// Render Object Information

	// textures
	GPUPixelBuffer m_GPUSpriteTextures;
	GPUPixelBuffer m_GPUFontTextures;

	// mesh textures
	InPlaceArray<Texture> m_MeshTextures = InPlaceArray<Texture>(RENDERER_MAXIMUM_TEXTURE_COUNT);
	queue<TextureDataTuple> m_MeshTextureUploadQueue;
	std::mutex m_MutexMeshTextureUpload;

	// sprites
	InPlaceArray<Sprite> m_Sprites = InPlaceArray<Sprite>(BUFFER_MAXIMUM_TEXTURE_COUNT);

	// text
	InPlaceArray<Font> m_Fonts = InPlaceArray<Font>(RENDERER_MAXIMUM_FONT_COUNT);
	list<Text> m_Texts;
	// FIXME font memory is too strict and i don't think this is a nice approach in this case

	// mesh
	list<ShaderPipeline> m_ShaderPipelines;
	list<GeometryBatch> m_GeometryBatches;
	list<ParticleBatch> m_ParticleBatches;
};

inline Renderer g_Renderer = Renderer();


#endif
