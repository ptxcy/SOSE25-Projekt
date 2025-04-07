#version 330 core


in vec2 EdgeCoordinates;

out vec4 pixelColour;


void main()
{
	pixelColour = vec4(EdgeCoordinates,.0,1.);
}
