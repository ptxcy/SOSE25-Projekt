#ifndef CORE_SHADER_HEADER
#define CORE_SHADER_HEADER


#include "base.h"
#include "buffer.h"


constexpr u32 SHADER_ERROR_LOGGING_LENGTH = 512;
constexpr size_t SHADER_UPLOAD_VALUE_SIZE = sizeof(float);


struct ShaderAttribute
{
	u8 dim;
	string name;
};

class Shader
{
public:
	static u32 compile(const char* path,GLenum type);
};

class VertexShader
{
public:
	VertexShader() {  }  // TODO remove this after pointing to the correct shader instead of copy
	VertexShader(const char* path);

public:
	u32 shader;
	vector<ShaderAttribute> vbo_attribs;
	vector<ShaderAttribute> ibo_attribs;
	size_t vbo_width = 0;
	size_t ibo_width = 0;

private:
	vector<ShaderAttribute>* write_head = &vbo_attribs;
	size_t* width_head = &vbo_width;
};

class FragmentShader
{
public:
	FragmentShader(const char* path);

public:
	u32 shader;
};


class ShaderPipeline
{
public:
	ShaderPipeline() {  }
	void assemble(VertexShader vs,FragmentShader& fs);
	void map(VertexBuffer* vbo,VertexBuffer* ibo=nullptr);

	// usage
	void enable();
	static void disable();

	// upload
	void upload(const char* varname,s32 value);
	void upload(const char* varname,f32 value);
	void upload(const char* varname,vec2 value);
	void upload(const char* varname,vec3 value);
	void upload(const char* varname,vec4 value);
	void upload(const char* varname,mat4 value);
	void upload_coordinate_system();
	void upload_camera();

private:
	void _define_attribute(ShaderAttribute& attrib);
	void _define_index_attribute(ShaderAttribute& attrib);
	s32 _handle_attribute_location_by_name(const char* varname);

private:

	// program
	u32 m_ShaderProgram;
	VertexShader m_VertexShader;

	// working iteration
	size_t m_VertexCursor = 0;
	size_t m_IndexCursor = 0;
};


#endif
