#include "buffer.h"


// ----------------------------------------------------------------------------------------------------
// Geometry Buffers


/**
 *	create vertex array
 */
VertexArray::VertexArray()
{
	glGenVertexArrays(1,&m_VAO);
}

/**
 *	bind vertex array
 */
void VertexArray::bind()
{
	glBindVertexArray(m_VAO);
}

/**
 *	unbind all vertex arrays
 */
void VertexArray::unbind()
{
	glBindVertexArray(0);
}


/**
 *	create vertex buffer
 */
VertexBuffer::VertexBuffer()
{
	glGenBuffers(1,&m_VBO);
}

/**
 *	bind vertex buffer
 */
void VertexBuffer::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER,m_VBO);
}

/**
 *	unbind all vertex buffers
 */
void VertexBuffer::unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER,0);
}

/**
 *	upload elements from array into buffer
 *	\param elements: array of optional element indices, mapping the vertex order
 *	\param size: size of array, holding the element indices
 *	NOTE vertex buffer has to be bound beforehand
 */
void VertexBuffer::upload_elements(u32* elements,size_t size)
{
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,size,elements,GL_STATIC_DRAW);
}

/**
 *	upload elements from vector into buffer
 *	\param elements: vector list of optional element indices to upload, mapping the vertex order
 *	NOTE vertex buffer has to be bound beforehand
 */
void VertexBuffer::upload_elements(std::vector<u32> elements)
{
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,elements.size()*sizeof(u32),&elements[0],GL_STATIC_DRAW);
}
