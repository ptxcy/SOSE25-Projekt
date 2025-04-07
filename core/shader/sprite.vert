#version 330 core


in vec2 position;
in vec2 edge_coordinates;

out vec2 EdgeCoordinates;


void main()
{
	EdgeCoordinates = edge_coordinates;
	gl_Position = vec4(position,.0,1.);
}
