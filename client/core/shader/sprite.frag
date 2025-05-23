#version 330 core


in vec2 EdgeCoordinates;
in float Alpha;

out vec4 pixelColour;

uniform sampler2D tex;


void main()
{
	//pixelColour = texture(tex,EdgeCoordinates);
	pixelColour = vec4(1,0,0,Alpha);
	//pixelColour.a *= Alpha;
}
