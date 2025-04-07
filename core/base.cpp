#include "base.h"


/**
 *	coordinate system setup for 2D objects
 *	\param xaxis: maximum range for visible geometry on x-axis
 *	\param yaxis: maximum range for visible geometry on y-axis
 */
CoordinateSystem2D::CoordinateSystem2D(f32 xaxis,f32 yaxis)
{
	view = glm::lookAt(vec3(0,-.0001f,1),vec3(.0f),vec3(0,0,1));
	proj = glm::ortho(.0f,xaxis,.0f,yaxis,.1f,10.f);
}
