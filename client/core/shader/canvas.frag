#version 330 core


in vec2 EdgeCoordinates;

out vec4 pixelColour;

uniform sampler2D tex;


void main()
{
	pixelColour = texture(tex,EdgeCoordinates);
}
