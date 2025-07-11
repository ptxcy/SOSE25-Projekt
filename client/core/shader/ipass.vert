#version 330 core


in vec3 position;
in vec2 uv;
in vec3 normal;
in vec3 tangent;

// engine: ibo
in vec3 offset;
in float scale;

out vec3 Position;
out vec3 Normal;

uniform mat4 view;
uniform mat4 proj;


void main()
{
	Position = position*scale+offset;
	Normal = normal;
	gl_Position = proj*view*vec4(Position,1.);
}
