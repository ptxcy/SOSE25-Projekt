#version 330 core


in vec2 EdgeCoordinates;

layout(location = 0) out vec4 pixelColour;

uniform sampler2D forward_target;
uniform sampler2D gbuffer_colour;
uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normals;
uniform sampler2D gbuffer_material;


uniform int switcher = 0;


void main()
{
	// map buffer components
	vec4 cmp_forward = texture(forward_target,EdgeCoordinates);
	vec4 cmp_colour = texture(gbuffer_colour,EdgeCoordinates);
	vec4 cmp_position = texture(gbuffer_position,EdgeCoordinates);
	vec4 cmp_normals = texture(gbuffer_normals,EdgeCoordinates);
	vec4 cmp_material = texture(gbuffer_material,EdgeCoordinates);
	vec4 cmps[5] = vec4[5](
		cmp_forward,
		cmp_colour,
		cmp_position,
		cmp_normals,
		cmp_material
	);

	// calculate final pixel colour
	pixelColour = cmps[switcher];
}
