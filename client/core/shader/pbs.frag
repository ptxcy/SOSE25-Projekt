#version 330 core


in vec2 EdgeCoordinates;

layout(location = 0) out vec4 pixelColour;

uniform sampler2D forward_target;
uniform sampler2D gbuffer_colour;
uniform sampler2D gbuffer_position;
uniform sampler2D gbuffer_normals;
uniform sampler2D gbuffer_material;
uniform sampler2D forward_depth;
uniform sampler2D gbuffer_depth;


void main()
{
	// map buffer components
	vec4 cmp_forward = texture(forward_target,EdgeCoordinates);
	vec4 cmp_colour = texture(gbuffer_colour,EdgeCoordinates);
	vec4 cmp_position = texture(gbuffer_position,EdgeCoordinates);
	vec4 cmp_normals = texture(gbuffer_normals,EdgeCoordinates);
	vec4 cmp_material = texture(gbuffer_material,EdgeCoordinates);
	float cmp_fdepth = texture(forward_depth,EdgeCoordinates).r;
	float cmp_gdepth = texture(gbuffer_depth,EdgeCoordinates).r;

	// calculate light
	vec3 final = cmp_colour.rgb;
	// TODO

	// calculate final pixel colour
	final = mix(final,cmp_forward.rgb,cmp_forward.a*int(cmp_fdepth<cmp_gdepth));
	pixelColour = vec4(final,1.);
}
