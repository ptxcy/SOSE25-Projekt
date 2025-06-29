#version 330 core

in vec3 Position;

layout(location = 0) out vec4 pixelColour;

uniform vec3 camera_position;

const float Scale = 1.025f;  // maybee more??

const float PI = 3.141592653;
const vec3 rB = vec3(5.8e-6,13.5e-6,33.1e-6);
const float rPIi = 1./(16.*PI);
const float rScl = 2e5;
const float mG = .76f;
const float mGSq = mG*mG;
const vec3 mB = vec3(.002);
const float mPIi = 1./(8.*PI);
const float mScl = 1e1;

const vec3 LightPosition = normalize(vec3(2000,2000,-2000));
// FIXME see clouds.frag, this should not be hardcoded, its dangerous and clumsy


void main()
{
	// phase calculations
	float cos_theta = dot(normalize(Position-camera_position),LightPosition);
	float ctsq = cos_theta*cos_theta;
	float rayleigh = 3.*rPIi*(1.+ctsq);
	float mie = 3.*mPIi*((1.-mGSq)*(1.+ctsq))/pow(1.+mGSq-2.*mG*cos_theta,1.5);
	// TODO cosine theta should be min'd for opposite hemisphere?

	// light scattering
	vec3 scatter = rB*rayleigh*rScl+mB*mie*mScl;
	scatter = vec3(1.)-exp(-scatter);

	// atmosphere fade
	float fade = clamp(dot(normalize(Position),normalize(camera_position)),.0,1.);
	fade = pow(1.-fade,5.);
	// TODO adjust sharpness

	// combine scattering & fade
	pixelColour = vec4(scatter,fade);
}
