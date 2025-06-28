#version 330 core


in vec3 Position;
in vec2 UV;
in vec3 Normal;

layout(location = 0) out vec4 pixelColour;

uniform sampler2D tex;

const vec3 LightPosition = vec3(2000,2000,-2000);
// FIXME assumes a static light. make light system also uploadable to other shader besides deferred light pass.


void main()
{
	float its = clamp(dot(Normal,LightPosition-Position),.05,1.);
	vec4 density = texture(tex,UV);
	pixelColour = vec4(density.rgb*its,density.a);
}
