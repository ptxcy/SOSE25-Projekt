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
	// TODO overload gpu_upload(...) for subtextures

private:
	string m_Path;
	s32 m_Width,m_Height;
	s32 m_Format;
	void* m_Data;
};


class Texture
{
public:
	Texture();

	void bind();
	static void unbind();

	void set_texture_parameter_linear_mipmap();
	void set_texture_parameter_nearest_mipmap();
	void set_texture_parameter_linear_unfiltered();
	void set_texture_parameter_nearest_unfiltered();
	void set_texture_parameter_clamp_to_edge();
	void set_texture_parameter_clamp_to_border();
	void set_texture_parameter_repeat();
	void set_texture_parameter_filter_bias(float bias=.0f);
	void set_texture_parameter_border_colour(vec4 colour);
	void generate_mipmap();

private:
	u32 m_Memory;
};


struct PixelBufferComponent
{
	u32 x,y;
	u32 width,height;
};

class GPUPixelBuffer
{
public:

	// TODO allocate & write for statically written texture atlas
	// TODO allocate & write for dynamically written texture atlas
	// TODO when allocating, rotation boolean can be stored in alpha by signing the float

public:
	Texture texture;
};

// TODO framebuffer

#endif
