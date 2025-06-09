#version 330 core


in vec2 UV;
in vec3 Normal;
in mat3 TBN;

layout(location = 0) out vec4 pixelColour;

uniform sampler2D tex;


void main()
{
	// load texture
	pixelColour = texture(tex,UV);
	pixelColour += textureLod(tex,UV,0)*(1.-pixelColour.a);

	// calculate lighting
	float its = dot(Normal,vec3(-1,0,0));
	pixelColour = clamp(its,.0,1.)*pixelColour;
}
