#include "shader.h"


// ----------------------------------------------------------------------------------------------------
// Shaders

/**
 *	compile given shader program
 *	\param path: path to shader program (can be vertex, fragment or geometry)
 *	\param type: shader type GL_(VERTEX+GEOMETRY+FRAGMENT)
 */
Shader::Shader(const char* path,GLenum type)
{
	COMM_AWT("compiling shader: %s",path);

	// open shader source
	std::ifstream __File(path);
	if (!__File)
	{
		COMM_ERR("no shader found at path: %s",path);
		return;
	}

	// read shader source
	string __SourceRaw;
	string __Line;
	while (!__File.eof())
	{
		std::getline(__File,__Line);
		__SourceRaw.append(__Line+'\n');
	}
	const char* __SourceCompile = __SourceRaw.c_str();
	__File.close();

	// compile shader
	shader = glCreateShader(type);
	glShaderSource(shader,1,&__SourceCompile,NULL);
	glCompileShader(shader);

	// compile error log
#ifdef DEBUG
	int __Status;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&__Status);
	if (!__Status)
	{
		char log[SHADER_ERROR_LOGGING_LENGTH];
		glGetShaderInfoLog(shader,SHADER_ERROR_LOGGING_LENGTH,NULL,log);
		COMM_ERR("[SHADER] %s -> %s",path,log);
	}
#endif

	COMM_CNF();
}


// ----------------------------------------------------------------------------------------------------
// Pipelines

/**
 *	assemble shader pipeline from compiled shaders
 *	pipeline flow: vertex shader -> (geometry shader) -> fragment shader
 *	\param vs: reference to compiled vertex shader
 *	\param fs: reference to compiled fragment shader
 *	\param vertex_width: width of the upload raster in vertex buffer
 *	\param index_width: width of the upload raster in index buffer
 *	\param name: name - id correlation for assembled shader pipeline in case of issues
 */
void ShaderPipeline::assemble(const Shader& vs,const Shader& fs,u8 vertex_width,u8 index_width,const char* name)
{
	// setup target width for vertex and index buffer
	m_VertexWidth = vertex_width*SHADER_UPLOAD_VALUE_SIZE;
	m_IndexWidth = index_width*SHADER_UPLOAD_VALUE_SIZE;

	// assemble program
	m_ShaderProgram = glCreateProgram();
	glAttachShader(m_ShaderProgram,vs.shader);
	glAttachShader(m_ShaderProgram,fs.shader);
	glBindFragDataLocation(m_ShaderProgram,0,"pixelColour");
	glLinkProgram(m_ShaderProgram);

	COMM_MSG(LOG_YELLOW,"%s -> Shader ID = %d",name,m_ShaderProgram);
}

/**
 *	enable/disable shader pipeline
 */
void ShaderPipeline::enable() { glUseProgram(m_ShaderProgram); }
void ShaderPipeline::disable() { glUseProgram(0); }

/**
 *	point to attribute in vertex buffer raster
 *	\param varname: variable name as defined as "in" in shader
 *	\param dim: variable dimension: 1 -> float, 2 -> vec2, 3 -> vec3, 4 -> vec4
 *	NOTE shader pipeline, vertex array & vertex buffer need to be active to point to attribute
 */
void ShaderPipeline::define_attribute(const char* varname,u8 dim)
{
	COMM_ERR_COND(m_VertexCursor+dim*SHADER_UPLOAD_VALUE_SIZE>m_VertexWidth,
				  "attribute dimension violates upload width");

	s32 __Attribute = _handle_attribute_location_by_name(varname);
	glVertexAttribPointer(__Attribute,dim,GL_FLOAT,GL_FALSE,m_VertexWidth,(void*)m_VertexCursor);
	m_VertexCursor += dim*SHADER_UPLOAD_VALUE_SIZE;
}

/**
 *	point to attribute in index buffer raster
 *	\param varname: variable name as defined as "in" in shader
 *	\param dim: variable dimension as already described in define_attribute(...) document
 *	NOTE shader pipeline, vertex array & index buffer need to be active to point to attribute
 */
void ShaderPipeline::define_index_attribute(const char* varname,u8 dim)
{
	COMM_ERR_COND(m_IndexCursor+dim*SHADER_UPLOAD_VALUE_SIZE>m_IndexWidth,
				  "index dimension violates upload width");

	s32 __Attribute = _handle_attribute_location_by_name(varname);
	glVertexAttribPointer(__Attribute,dim,GL_FLOAT,GL_FALSE,m_IndexWidth,(void*)m_IndexCursor);
	glVertexAttribDivisor(__Attribute,1);
	m_IndexCursor += dim*SHADER_UPLOAD_VALUE_SIZE;
}

/**
 *	upload uniform variable to shader
 *	\param un: variable name as defined as "uniform" in shader (must be part of the pipeline)
 *	\param uv: value to upload to specified variable
 *	NOTE shader pipeline needs to be active to upload values to uniform variables
 */
void ShaderPipeline::upload(const char* varname,s32 value)
	{ glUniform1i(glGetUniformLocation(m_ShaderProgram,varname),value); }
void ShaderPipeline::upload(const char* varname,f32 value)
	{ glUniform1f(glGetUniformLocation(m_ShaderProgram,varname),value); }
void ShaderPipeline::upload(const char* varname,vec2 value)
	{ glUniform2f(glGetUniformLocation(m_ShaderProgram,varname),value.x,value.y); }
void ShaderPipeline::upload(const char* varname,vec3 value)
	{ glUniform3f(glGetUniformLocation(m_ShaderProgram,varname),value.x,value.y,value.z); }
void ShaderPipeline::upload(const char* varname,vec4 value)
	{ glUniform4f(glGetUniformLocation(m_ShaderProgram,varname),value.x,value.y,value.z,value.w); }
void ShaderPipeline::upload(const char* varname,mat4 value)
	{ glUniformMatrix4fv(glGetUniformLocation(m_ShaderProgram,varname),1,GL_FALSE,glm::value_ptr(value)); }

/**
 *	automatically upload the global 2D coordinate system to the shader
 *	the coordinate system is uploaded to uniforms view = "view", proj = "proj"
 */
void ShaderPipeline::upload_coordinate_system()
{
	upload("view",g_CoordinateSystem.view);
	upload("proj",g_CoordinateSystem.proj);
}

/**
 *	input attribute name and receive the attribute id
 *	\param name of the vertex/index attribute
 */
s32 ShaderPipeline::_handle_attribute_location_by_name(const char* varname)
{
	s32 attribute = glGetAttribLocation(m_ShaderProgram,varname);
	glEnableVertexAttribArray(attribute);
	return attribute;
}
