#ifndef CORE_SHADER_HEADER
#define CORE_SHADER_HEADER


#include "base.h"


constexpr u32 SHADER_ERROR_LOGGING_LENGTH = 512;
constexpr size_t SHADER_UPLOAD_VALUE_SIZE = sizeof(float);


class Shader
{
public:
	Shader(const char* path,GLenum type);

public:
	u32 shader;
};


class ShaderPipeline
{
public:
	ShaderPipeline() {  }

	void assemble(const Shader& vs,const Shader& fs,u8 vertex_width,u8 index_width,const char* name);

	void enable();
	static void disable();

	void define_attribute(const char* varname,u8 dim);
	void define_index_attribute(const char* varname,u8 dim);

	void upload(const char* varname,s32 value);
	void upload(const char* varname,f32 value);
	void upload(const char* varname,vec2 value);
	void upload(const char* varname,vec3 value);
	void upload(const char* varname,vec4 value);
	void upload(const char* varname,mat4 value);
	void upload_coordinate_system();
	void upload_camera();

private:
	s32 _handle_attribute_location_by_name(const char* varname);

private:
	u32 m_ShaderProgram;

	size_t m_VertexWidth;
	size_t m_IndexWidth;

	size_t m_VertexCursor = 0;
	size_t m_IndexCursor = 0;
};


#endif
