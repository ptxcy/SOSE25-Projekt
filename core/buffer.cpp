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
	COMM_ERR_COND(!check_file_exists(m_Path.c_str()),"texture %s could not be found",m_Path.c_str());
	m_Data = stbi_load(m_Path.c_str(),&m_Width,&m_Height,0,STBI_rgb_alpha);
}
// TODO multithread by default and forget about it

/**
 *	upload data to gpu
 *	NOTE has to be uploaded on main thread
 *	NOTE target texture has to be bound before uploading
 */
void TextureData::gpu_upload()
{
	glTexImage2D(GL_TEXTURE_2D,0,m_Format,m_Width,m_Height,0,GL_RGBA,GL_UNSIGNED_BYTE,m_Data);
	stbi_image_free(m_Data);
}
// TODO move pixel free from gpu upload to reduce weight on main thread occupied by context

/**
 *	upload data as subtexture to atlas on gpu
 *	TODO
 *	NOTE has to be uploaded in main thread
 *	NOTE target texture has to be bound and allocated before uploading
 */
void TextureData::gpu_upload(u32 x,u32 y)
{
	glTexSubImage2D(GL_TEXTURE_2D,0,x,y,m_Width,m_Height,m_Format,GL_UNSIGNED_BYTE,m_Data);
	stbi_image_free(m_Data);
}
// TODO move pixel free from gpu upload to reduce weight on main thread occupied by context

/**
 *	calculate relative dimensions towards the given pixel buffer atlas
 *	\param idim: inverted atlas dimensions
 *	\returns dimensions clamped between 0,1 as edges of the pixel buffer
 */
vec2 TextureData::calculate_relative_dimensions(vec2 idim)
{
	return vec2(m_Width,m_Height)*idim;
}


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


/**
 *	allocate video memory to use as we, the programmers please
 *	\param width: buffer width
 *	\param height: buffer height
 *	\param format: colourspace format of pixels
 *	NOTE cannot be executed in subthread, uses context bound to main thread
 */
void GPUPixelBuffer::allocate(u32 width,u32 height,u32 format)
{
	// store info
	m_Format = format;
	m_Dimensions = vec2(width,height);
	m_InvDimensions = vec2(1.f/width,1.f/height);

	// allocate memory
	glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,0);
}
// FIXME in case of sRGB colourspace for example the format in internalformat and format won't be the same
//		furthermore this is absolutely necessary to be of dynamic nature, due to monocrome buffers being a thing

/**
 *	write texture buffer to preallocated gpu memory
 *	\param comp: pixel buffer component data, will be uploaded to gpu, to locate correct position on atlas
 *	\param data: texture data holding pixel buffer to write to subtexture as well as dimensions
 *	NOTE related texture has to be bound beforehand
 */
void GPUPixelBuffer::write(PixelBufferComponent* comp,TextureData* data)
{
	u32 x = 0;
	u32 y = 0;
	// TODO actually calculate position of new texture based on the current atlas state

	// write subtexture info
	comp->position = vec2(x,y)*m_InvDimensions;
	comp->dimensions = data->calculate_relative_dimensions(m_InvDimensions);

	// write buffer
	data->gpu_upload(x,y);
}
// TODO create a gpu_write and only upload (& bind) in that function, then do the location code in threadable
//		capsulated method, storing all necessary information in the pixel buffer component
