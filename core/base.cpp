#include "base.h"


// ----------------------------------------------------------------------------------------------------
// Utility

/**
 *	check if a file can be found at given location
 *	\param path: path to check
 *	\returns true if file could be accessed
 *	NOTE for use in debug cases only, in release checking for data that is guaranteed is not good practice
 */
bool check_file_exists(const char* path)
{
	struct stat tf;
	return stat(path,&tf)==0;
}


// ----------------------------------------------------------------------------------------------------
// Low-Level Data

/**
 *	TODO
 */
Bytes::Bytes(size_t bytes)
	: m_Size(bytes)
{
	m_Data = (u8*)malloc(bytes);
	reset();
}

/**
 *	TODO
 */
Bytes::~Bytes()
{
	free(m_Data);
}


// ----------------------------------------------------------------------------------------------------
// Coordinate System

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
