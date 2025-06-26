#version 330 core


in vec3 position;
in vec2 uv;
in vec3 normal;
in vec3 tangent;
// FIXME most of this can be removed for performance gains

out vec2 UV;

uniform float disp = 2.;
uniform float scale = 1.;

uniform mat4 view;
uniform mat4 proj;


void main()
{
	vec3 Position = vec3(position.x+disp,position.y+disp,position.z);
	gl_Position = proj*view*vec4(Position*scale,1.);
	UV = uv;
}
