#ifndef CORE_BUFFER_HEADER
#define CORE_BUFFER_HEADER


#include "base.h"


class VertexArray
{
public:
	VertexArray();

	void bind();
	static void unbind();

private:
	u32 m_VAO;
};


class VertexBuffer
{
public:
	VertexBuffer();

	void bind();
	static void unbind();

	/**
	 *	template inline for dynamic vertex struct uploads
	 *	\param vertices: vertex array/vector holding geometry
	 *	\param size: array size, not necessary when using a vector
	 *	\param memtype: GL_(STREAM+STATIC+DYNAMIC)_(DRAW+READ+COPY)
	 *	NOTE vertex buffer has to be bound beforehand
	 *	NOTE do not use in combination with upload_elements(...)
	 */
	template<typename T> inline void upload_vertices(T* vertices,size_t size,GLenum memtype=GL_STATIC_DRAW)
	{ glBufferData(GL_ARRAY_BUFFER,size*sizeof(T),vertices,memtype); }
	template<typename T> inline void upload_vertices(std::vector<T> vertices,GLenum memtype=GL_STATIC_DRAW)
	{ glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(T),&vertices[0],memtype); }

	void upload_elements(u32* elements,size_t size);
	void upload_elements(std::vector<u32> elements);

private:
	u32 m_VBO;
};


struct TextureData
{
public:
	TextureData(s32 format=GL_RGBA);

	void load(string& path);
	void gpu_upload();
	void gpu_upload_subtexture();

private:
	void _free();

public:
	u32 x,y;
	s32 width,height;
	u8* data;

private:
	s32 m_Format;
	bool m_TextureFlag = false;
};


class Texture
{
public:
	Texture();

	void bind();
	static void unbind();

	static void set_texture_parameter_linear_mipmap();
	static void set_texture_parameter_nearest_mipmap();
	static void set_texture_parameter_linear_unfiltered();
	static void set_texture_parameter_nearest_unfiltered();
	static void set_texture_parameter_clamp_to_edge();
	static void set_texture_parameter_clamp_to_border();
	static void set_texture_parameter_repeat();
	static void set_texture_parameter_filter_bias(float bias=.0f);
	static void set_texture_parameter_border_colour(vec4 colour);
	static void generate_mipmap();

private:
	u32 m_Memory;
};


struct PixelBufferComponent
{
	vec2 offset;
	vec2 dimensions;
};

struct Glyph
{
	vec2 scale;
	vec2 bearing;
	s64 advance;
};

struct Font
{
	PixelBufferComponent tex[96];
	Glyph glyphs[96];
	u16 size;
};

constexpr u16 BUFFER_MAXIMUM_TEXTURE_COUNT = 1024;
constexpr u8 BUFFER_ATLAS_BORDER_PADDING = 2;
struct GPUPixelBuffer
{
	// utilty
	void allocate(u32 width,u32 height,u32 format);
	static void load_texture(GPUPixelBuffer* gpb,PixelBufferComponent* pbc,string path);
	static void load_font(GPUPixelBuffer* gpb,Font* font,const char* path,u16 size);
	static void _load(GPUPixelBuffer* gpb,PixelBufferComponent* pbc,TextureData* data);
	void gpu_upload();
	// TODO allocate & write for statically written texture atlas
	// TODO when allocating, rotation boolean can be stored in alpha by signing the float
	// TODO allow to merge deleted rects when using a dynamic texture atlas
	// FIXME format can be assigned when allocating but load instructions are format dependent

	// data
	Texture atlas;
	vec2 dimensions_inv;
	std::vector<PixelBufferComponent> memory_segments;
	std::mutex mutex_memory_segments;
	InPlaceArray<PixelBufferComponent> textures
			= InPlaceArray<PixelBufferComponent>(BUFFER_MAXIMUM_TEXTURE_COUNT);
	std::mutex mutex_texture_requests;
	std::queue<TextureData> load_requests;
	ThreadSignal signal;
};


// TODO framebuffer

#endif
