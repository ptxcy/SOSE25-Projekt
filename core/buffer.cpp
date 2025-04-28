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
	m_Data = stbi_load(m_Path.c_str(),&width,&height,0,STBI_rgb_alpha);
}

/**
 *	upload data to gpu
 *	NOTE has to be uploaded on main thread
 *	NOTE target texture has to be bound before uploading
 */
void TextureData::gpu_upload()
{
	glTexImage2D(GL_TEXTURE_2D,0,m_Format,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,m_Data);
	stbi_image_free(m_Data);
}

/**
 *	upload data as subtexture to atlas on gpu
 *	\param x: x-axis atlas location of the subtexture
 *	\param y: y-axis atlas location of the subtexture
 *	NOTE has to be uploaded in main thread
 *	NOTE target texture has to be bound and allocated before uploading
 */
void TextureData::gpu_upload(u32 x,u32 y)
{
	glTexSubImage2D(GL_TEXTURE_2D,0,x,y,width,height,m_Format,GL_UNSIGNED_BYTE,m_Data);
	stbi_image_free(m_Data);
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
 *	NOTE pointer trick not field tested, should border colour fail to work this is the most likely culprit
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
	//m_Format = format;
	//m_Dimensions = vec2(width,height);
	dimensions_inv = vec2(1.f/width,1.f/height);

	// allocate memory
	memory_segments.push_back({
			.offset = vec2(0,0),
			.dimensions = vec2(width,height)
		});
	glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,0);
}
// FIXME inconsistent formatting, the allocation format can easily be detached from texture data structure
// FIXME in case of sRGB colourspace for example the format in internalformat and format won't be the same
//		furthermore this is absolutely necessary to be of dynamic nature, due to monocrome buffers being a thing

/**
 *	write texture buffer to preallocated gpu memory
 *	\param gpb: target pixel buffer
 *	\param requests: pointer to load request queue, receiving the gpu upload request when loading is done
 *	\param pbc: pointer to atlas component information, this will be overwritten
 *	\param mutex_requests: simple mutual exclusion, blocking the access to the load request queue
 *	\param path: path to texture file
 *	NOTE this is supposed to run as a subthread, hence the mutex and load request queue pointer
 */
std::mutex _mutex_memory_segments;
void GPUPixelBuffer::load(GPUPixelBuffer* gpb,std::queue<TextureData>* requests,PixelBufferComponent* pbc,
						  std::mutex* mutex_requests,const char* path)
{
	// load information from texture file
	TextureData __TextureData = TextureData(path);
	__TextureData.load();

	// locate best position for texture on free memory space
	f32 __BestDifference = 0x7f800000;
	u32 __MemoryIndex = -1;
	for (u32 i=0;i<gpb->memory_segments.size();i++)
	{
		PixelBufferComponent* p_FreeComponent = &gpb->memory_segments[i];
		if (__TextureData.width>p_FreeComponent->dimensions.x
			||__TextureData.height>p_FreeComponent->dimensions.y) continue;

		// find closest fit
		f32 __AreaDifference = p_FreeComponent->dimensions.x*p_FreeComponent->dimensions.y
				- __TextureData.width*__TextureData.height;
		if (__AreaDifference<__BestDifference)
		{
			__MemoryIndex = i;
			__BestDifference = __AreaDifference;
		}
	}

	// get memory segment pointer
	COMM_ERR_COND(__MemoryIndex==-1,"sprite texture memory is populated or segmented. texture upload failed!");
	PixelBufferComponent* p_CloseFitComponent = &gpb->memory_segments[__MemoryIndex];

	// write atlas information
	__TextureData.x = p_CloseFitComponent->offset.x, __TextureData.y = p_CloseFitComponent->offset.y;
	pbc->offset = p_CloseFitComponent->offset*gpb->dimensions_inv;
	pbc->dimensions = vec2(__TextureData.width,__TextureData.height)*gpb->dimensions_inv;

	// segment free memory to reserve pixel space for upload
	PixelBufferComponent __Side = {
		.offset = p_CloseFitComponent->offset+vec2(__TextureData.width,0),
		.dimensions = vec2(p_CloseFitComponent->dimensions.x-__TextureData.width,__TextureData.height)
	};
	PixelBufferComponent __Below = {
		.offset = p_CloseFitComponent->offset+vec2(0,__TextureData.height),
		.dimensions = p_CloseFitComponent->dimensions-vec2(0,__TextureData.height)
	};
	// FIXME this is segmenting falsely, it's not possible to insert into texture space that has the correct
	//		dimensions in only one segment but crosses over into a different free rect.
	//		alternatively this can be done by assigning cross segment in both subsequent segments, but
	//		this will mess with memory information, due to multiple free states per pixel. geez louize
	//		solve this with a consistent merger algorithm after every segment?

	// update memory information data
	_mutex_memory_segments.lock();
	gpb->memory_segments.erase(gpb->memory_segments.begin()+__MemoryIndex);
	if (__Side.dimensions.x>0) gpb->memory_segments.push_back(__Side);
	if (__Below.dimensions.y>0) gpb->memory_segments.push_back(__Below);
	_mutex_memory_segments.unlock();
	// TODO when deleting and segmenting, check if free subspaces can be merged back into each other
	// FIXME filter bleed, throw in a padding border, so the neighbouring textures don't flow into each other

	// write buffer
	mutex_requests->lock();
	requests->push(__TextureData);
	mutex_requests->unlock();
}
