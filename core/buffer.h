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


class TextureData
{
public:
	TextureData(string path,bool corrected=false);

	void load();
	void gpu_upload();
	void gpu_upload(u32 x,u32 y);

public:
	u32 x,y;
	s32 width,height;

private:
	string m_Path;
	s32 m_Format;
	void* m_Data;
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
	vec2 position;
	vec2 dimensions;
};

struct GPUPixelBuffer
{
	// utilty
	void allocate(u32 width,u32 height,u32 format);
	static void load(GPUPixelBuffer* gpb,std::queue<TextureData>* requests,
					 std::mutex* mutex_requests,const char* path);
	// TODO allocate & write for statically written texture atlas
	// TODO allocate & write for dynamically written texture atlas
	// TODO when allocating, rotation boolean can be stored in alpha by signing the float

	// data
	Texture atlas;
	vec2 dimensions_inv;
	std::vector<PixelBufferComponent> memory_segments;
	//u32 m_Format;
	//vec2 m_Dimensions;
};

// TODO framebuffer

#endif
