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


// TODO texture buffer / VRAM buffer
// TODO framebuffer


#endif
