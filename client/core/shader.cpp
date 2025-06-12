#include "shader.h"


// ----------------------------------------------------------------------------------------------------
// Shaders

/**
 *	compile given shader program
 *	\param path: path to shader program (can be vertex, fragment or geometry)
 *	\param type: shader type GL_(VERTEX+GEOMETRY+FRAGMENT)
 *	\returns compiled shader pipeline fragment
 */
u32 Shader::compile(const char* path,GLenum type)
{
	COMM_AWT("compiling shader: %s",path);

	// open shader source
	std::ifstream __File(path);
	if (!__File)
	{
		COMM_ERR("no shader found at path: %s",path);
		return 0;
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
	u32 shader = glCreateShader(type);
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
	return shader;
}

/**
 *	create a vertex shader from source
 *	\param path: path to GLSL vertex source file
 */
VertexShader::VertexShader(const char* path)
{
	shader = Shader::compile(path,GL_VERTEX_SHADER);
	if (!shader)
	{
		COMM_ERR("[SHADER] skipping input parser, vertex shader is corrupted");
		return;
	}

	// assess input pattern for vbo/ibo automapping
	std::ifstream __File(path);
	string __Line;
	while (!__File.eof())
	{
		std::getline(__File,__Line);
		if (__Line.find("// engine: ibo")==0)
		{
			write_head = &ibo_attribs;
			width_head = &ibo_width;
			continue;
		}
		else if (__Line.find("in")!=0) continue;
		else if (__Line.find("void main()")==0) break;

		// extract input information
		std::stringstream str(__Line);
		string token;
		vector<string> tokens;
		while (str>>token) tokens.push_back(token);
		tokens[2].pop_back();

		// interpret input definition line
		u8 dim = (tokens[1]=="float") ? 1 : tokens[1][3]-0x30;
		write_head->push_back({ dim,tokens[2] });
		(*width_head) += dim;
	}

	// convert widths to byte format
	vbo_width *= SHADER_UPLOAD_VALUE_SIZE;
	ibo_width *= SHADER_UPLOAD_VALUE_SIZE;
}

/**
 *	create a fragment shader from source
 *	\param path: path to GLSL fragment source file
 */
FragmentShader::FragmentShader(const char* path)
{
	shader = Shader::compile(path,GL_FRAGMENT_SHADER);
}


// ----------------------------------------------------------------------------------------------------
// Pipelines

/**
 *	assemble shader pipeline from compiled shaders
 *	pipeline flow: vertex shader -> (geometry shader) -> fragment shader
 *	\param vs: compiled vertex shader
 *	\param fs: reference to compiled fragment shader
 */
void ShaderPipeline::assemble(VertexShader vs,FragmentShader& fs)
{
	m_VertexShader = vs;
	// FIXME this CAN and SHOULD be critisized! awful memory management through heavy copy!

	// assemble program
	m_ShaderProgram = glCreateProgram();
	glAttachShader(m_ShaderProgram,vs.shader);
	glAttachShader(m_ShaderProgram,fs.shader);
	glLinkProgram(m_ShaderProgram);
}

/**
 *	automatically map vertex and index buffer object to vertex shader input
 *	\param vbo: vertex buffer object
 *	\param ibo: (default nullptr) index buffer object
 *	NOTE vertex buffer needs to be active
 */
void ShaderPipeline::map(VertexBuffer* vbo,VertexBuffer* ibo)
{
	// vertex buffer
	COMM_LOG("mapping shader (vbo = %lu:%lu,ibo = %lu:%lu)",
			 m_VertexShader.vbo_attribs.size(),m_VertexShader.vbo_width,
			 m_VertexShader.ibo_attribs.size(),m_VertexShader.ibo_width);
	enable();
	for (ShaderAttribute& attrib : m_VertexShader.vbo_attribs) _define_attribute(attrib);
	m_VertexCursor = 0;

	// index buffer
	if (ibo==nullptr||!m_VertexShader.ibo_attribs.size()) return;
	ibo->bind();
	for (ShaderAttribute& attrib : m_VertexShader.ibo_attribs) _define_index_attribute(attrib);
	m_IndexCursor = 0;
}

/**
 *	enable/disable shader pipeline
 */
void ShaderPipeline::enable() { glUseProgram(m_ShaderProgram); }
void ShaderPipeline::disable() { glUseProgram(0); }

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
 *	automatically upload the global 3D camera to the shader
 *	the camera is uploaded to uniforms view = "view", proj = "proj"
 */
void ShaderPipeline::upload_camera()
{
	upload("view",g_Camera.view);
	upload("proj",g_Camera.proj);
}

/**
 *	point to attribute in vertex buffer raster
 *	\param attrib: shader attribute structure, holding attribute name and dimension
 *	NOTE shader pipeline, vertex array & vertex buffer need to be active to point to attribute
 */
void ShaderPipeline::_define_attribute(ShaderAttribute& attrib)
{
	COMM_ERR_COND(m_VertexCursor+attrib.dim*SHADER_UPLOAD_VALUE_SIZE>m_VertexShader.vbo_width,
				  "attribute dimension violates upload width");

	s32 __Attribute = _handle_attribute_location_by_name(attrib.name.c_str());
	glVertexAttribPointer(__Attribute,attrib.dim,GL_FLOAT,GL_FALSE,
						  m_VertexShader.vbo_width,(void*)m_VertexCursor);
	m_VertexCursor += attrib.dim*SHADER_UPLOAD_VALUE_SIZE;
}

/**
 *	point to attribute in index buffer raster
 *	\param attrib: shader attribute structure, holding attribute name and dimension
 *	NOTE shader pipeline, vertex array & index buffer need to be active to point to attribute
 */
void ShaderPipeline::_define_index_attribute(ShaderAttribute& attrib)
{
	COMM_ERR_COND(m_IndexCursor+attrib.dim*SHADER_UPLOAD_VALUE_SIZE>m_VertexShader.ibo_width,
				  "index dimension violates upload width");

	s32 __Attribute = _handle_attribute_location_by_name(attrib.name.c_str());
	glVertexAttribPointer(__Attribute,attrib.dim,GL_FLOAT,GL_FALSE,
						  m_VertexShader.ibo_width,(void*)m_IndexCursor);
	glVertexAttribDivisor(__Attribute,1);
	m_IndexCursor += attrib.dim*SHADER_UPLOAD_VALUE_SIZE;
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
