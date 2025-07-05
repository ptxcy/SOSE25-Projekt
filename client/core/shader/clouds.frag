#version 330 core


in vec3 Position;
in vec2 UV;
in vec3 Normal;

layout(location = 0) out vec4 pixelColour;

uniform sampler2D tex;

uniform vec3 light_position;


void main()
{
	float light_intensity = dot(Normal,normalize(light_position-Position));
	light_intensity = pow(light_intensity,.4f);
	vec4 density = texture(tex,UV);
	pixelColour = vec4(density.rgb*light_intensity,length(density.rgb));
}
// FIXME now it looks awful. more correct but awful!
