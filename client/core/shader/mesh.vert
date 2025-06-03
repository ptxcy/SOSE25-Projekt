#version 330 core


in vec3 position;
in vec2 uv;
in vec3 normal;
in vec3 tangent;

out vec3 Normal;
//out mat3 TBN;

uniform mat4 view;
uniform mat4 proj;


void main()
{
	gl_Position = proj*view*vec4(position,1.);
	Normal = normal;
	/*
	vec3 Tangent = normalize(tangent-dot(tangent,normal)*normal);
	TBN = mat3(Tangent,cross(normal,Tangent),normal);
	*/
}
