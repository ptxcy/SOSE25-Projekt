#version 330 core


in vec3 position;
in vec2 edge_coordinates;
in vec3 normals;
in vec3 tangent;

out vec3 Position;
out vec2 EdgeCoordinates;
out mat3 TBN;

uniform mat4 view;
uniform mat4 proj;

uniform vec3 offset = vec3(0);
uniform float texel = 1.;


void main()
{
	Position = position+offset;
	gl_Position = proj*view*vec4(Position,1.);

	// calculate texture coordinates
	EdgeCoordinates = edge_coordinates*texel;

	// gram-schmidt reorthogonalization
	vec3 Tangent = normalize(tangent-dot(tangent,normals)*normals);
	TBN = mat3(Tangent,cross(normals,Tangent),normals);
}
