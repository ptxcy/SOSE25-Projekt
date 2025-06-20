#version 330 core


in vec3 position;
in vec2 uv;
in vec3 normal;
in vec3 tangent;
// FIXME most of this can be removed for performance gains

out vec2 UV;

uniform float scale = 1.f;

uniform mat4 view;
uniform mat4 proj;


void main()
{
	gl_Position = proj*view*vec4(position*scale,1.);
	UV = uv;
}
