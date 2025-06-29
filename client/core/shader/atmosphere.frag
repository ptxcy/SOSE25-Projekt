#version 330 core

in vec3 Position;

layout(location = 0) out vec4 pixelColour;

uniform vec3 camera_position;

// atmosphere radius
const float Scale = 1.05f;  // maybee more??

// scattering constants
const float PI = 3.141592653;
const vec3 rB = vec3(5.8e-6,13.5e-6,33.1e-6);
const float rPIi = 3./(16.*PI);
const float rScl = 2e5;
const float mG_air = .76f;
const float mGSq = mG_air*mG_air;
const vec3 mB = vec3(.002);
const float mPIi = 3./(8.*PI);
const float mScl = 1e1;
const float exposure = 1.f;

const vec3 LightPosition = normalize(vec3(2000,2000,-2000));
// FIXME see clouds.frag, this should not be hardcoded, its dangerous and clumsy


void main()
{
	// precalculation
	vec3 camera_direction = normalize(Position-camera_position);
	vec3 norm_position = normalize(Position);
	float geom_factor = clamp(dot(norm_position,LightPosition),.2,1.);

	// pseudo path-tracing through atmosphere utilizing linear algebra
	// intersection with atmosphere
	float a = 1.;
	float b = 2.*dot(camera_direction,camera_position);
	float c = dot(camera_position,camera_position)-Scale*Scale;
	float outer_discr = b*b-4.*a*c;
	float sq_discr = sqrt(outer_discr);
	float t0 = (-b-sq_discr)*.5;
	float t1 = (-b+sq_discr)*.5;

	// intersection with planet
	float c_planet = dot(camera_position,camera_position)-1.;
	float inner_discr = b*b-4.*a*c_planet;
	if (inner_discr>.0)
	{
		sq_discr = sqrt(inner_discr);
		float t0i = (-b-sq_discr)*.5;
		float t1i = (-b+sq_discr)*.5;
		float tsurface = min(t0i,t1i);
		t1 = (tsurface>t0&&tsurface<t1) ? tsurface : t1;
	}
	float ray_distance = t1-t0;
	pixelColour = vec4(vec3(ray_distance),1.);
	return;

	// phase calculations
	float cos_theta = dot(camera_direction,LightPosition);
	float ctsq = cos_theta*cos_theta;
	float rayleigh = rPIi*(1.+ctsq);
	float mie = mPIi*((1.-mGSq)*(1.+ctsq))/pow(1.+mGSq-2.*mG_air*cos_theta,1.5);

	// light scattering
	vec3 scatter = rB*rayleigh*rScl+mB*mie*mScl;
	scatter = vec3(1.)-exp(-scatter*exposure);

	// atmosphere fade
	float fade = (1-clamp(dot(norm_position,normalize(camera_position)),.0,1.))*geom_factor;
	fade = max(pow(fade,1),.1f);

	// combine scattering & fade
	pixelColour = vec4(scatter,fade);
}
