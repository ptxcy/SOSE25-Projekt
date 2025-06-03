#version 330 core


//in mat3 TBN;
in vec3 Normal;

out vec4 pixelColour;

uniform sampler2D tex;


void main()
{
	float its = dot(Normal,vec3(0,1,.2));
	pixelColour = clamp(its,.0,1.)*vec4(.85,0,0,1.);
}
