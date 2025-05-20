#version 330 core


in vec2 position;
in vec2 edge_coordinates;

in vec2 offset;
in float scale;
in vec4 colour;
in vec2 atlas_position;
in vec2 atlas_dimension;

out vec2 EdgeCoordinates;
out vec4 Colour;

uniform mat4 view;
uniform mat4 proj;


void main()
{
	// pass
	gl_Position = proj*view*vec4(position*scale/*+offset*/,.0,1.);
	EdgeCoordinates = edge_coordinates;
	Colour = colour;
}
