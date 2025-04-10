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


// ----------------------------------------------------------------------------------------------------
// Video Buffers

/**
 *	allocation and setup for texture data load
 *	\param path: path to texture file
 *	\param corrected: (default false) true if texture is corrected sRGB colourspace instead of regular RGBA
 */
TextureData::TextureData(string path,bool corrected)
	: m_Path(path)
{
	m_Format = GL_RGBA+corrected*0x7338;
}

/**
 *	make the cpu load the texture data & dimensions from file
 */
void TextureData::load()
{
	m_Data = stbi_load(m_Path.c_str(),&m_Width,&m_Height,0,STBI_rgb_alpha);
}
// TODO multithread by default and forget about it

/**
 *	upload data to gpu
 *	NOTE has to be uploaded on main thread
 *	NOTE target texture has to be bound when uploading
 */
void TextureData::gpu_upload()
{
	glTexImage2D(GL_TEXTURE_2D,0,m_Format,m_Width,m_Height,0,GL_RGBA,GL_UNSIGNED_BYTE,m_Data);
	stbi_image_free(m_Data);
}
// TODO move pixel free from gpu upload to reduce weight on main thread occupied by context


/**
 *	setup texture buffer
 */
Texture::Texture()
{
	glGenTextures(1,&m_Memory);
}

/**
 *	bind texture buffer for read and write procedures
 */
void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D,m_Memory);
}

/**
 *	release any bound textures
 */
void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D,0);
}

/**
 *	define trilinear texture filter for mipmap generation
 *	NOTE texture should be bound
 *	NOTE this allows generate_mipmap() to be executed if no manual approach is chosen
 */
void Texture::set_texture_parameter_linear_mipmap()
{
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

/**
 *	define bilinear texture filter for mipmap generation
 *	NOTE texture should be bound
 *	NOTE this allows generate_mipmap() to be executed if no manual approach is chosen
 */
void Texture::set_texture_parameter_nearest_mipmap()
{
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
}

/**
 *	define linear filter without mipmapping
 *	NOTE texture should be bound
 */
void Texture::set_texture_parameter_linear_unfiltered()
{
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

/**
 *	define filterless pixelraster
 *	NOTE texture should be bound
 */
void Texture::set_texture_parameter_nearest_unfiltered()
{
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
}

/**
 *	define extension behaviour to stretch to maximum size
 *	NOTE texture should be bound
 */
void Texture::set_texture_parameter_clamp_to_edge()
{
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
}

/**
 *	define extension behaviour to be scaled towards custom borders, avoiding ratio manipulation
 *	NOTE texture should be bound
 */
void Texture::set_texture_parameter_clamp_to_border()
{
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
}

/**
 *	repeat texture over vertices without scaling
 *	NOTE texture should be bound
 */
void Texture::set_texture_parameter_repeat()
{
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
}

/**
 *	define level of detail through filter bias (will additively shift the perceived lod value)
 *	\param bias: (default .0f) filter bias, addition bias means that .0f is no bias
 *	NOTE texture should be bound
 */
void Texture::set_texture_parameter_filter_bias(float bias)
{
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_LOD_BIAS,bias);
}

/**
 *	define border colour for texture when set_texture_parameter_clamp_to_border() is defined
 *	\param colour: RGBA border colour as vector
 *	NOTE texture should be bound
 *	TODO pointer trick not field tested, should border colour fail to work this is the most likely culprit
 */
void Texture::set_texture_parameter_border_colour(vec4 colour)
{
	glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,&colour.r);
}

/**
 *	automatically generate mipmap when the appropriate texture parameters are set (_mipmap() suffix)
 *	NOTE texture should be bound
 */
void Texture::generate_mipmap()
{
	glGenerateMipmap(GL_TEXTURE_2D);
}
