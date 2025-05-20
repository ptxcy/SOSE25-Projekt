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
 *	memory allocation for requested amount of bits with subsequent neutralization
 *	\param size: size of bit sequence. this is the exact amount of needed bits, not in fact a bytelength
 */
BitwiseWords::BitwiseWords(size_t size)
	: m_Size(size/sizeof(__system_word)+1)
{
	m_Data = (__system_word*)malloc(m_Size);
	reset();
}

/**
 *	automatically release the allocated data bits on destruction
 */
BitwiseWords::~BitwiseWords()
{
	free(m_Data);
}


// ----------------------------------------------------------------------------------------------------
// Signals Between Threads

/**
 *	start to wait for a continue signal
 */
void ThreadSignal::wait()
{
	std::unique_lock<std::mutex> lock(mutex);
	cv.wait(lock,[this]{ return active; });
}

/**
 *	signal all threads listening to continue with procedure
 */
void ThreadSignal::proceed()
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		active = true;
	}
	cv.notify_one();
}

/**
 *	signal subprocess to vanish
 */
void ThreadSignal::exit()
{
	running = false;
	proceed();
}


// ----------------------------------------------------------------------------------------------------
// Geometry

/**
 *	determines if the given point is intersecting with the rect
 *	\param point: point in 2D space to check relationship with rect
 *	\returns true if point is inside rect bounds
 */
bool Rect::intersect(vec2 point)
{
	return (point.x<extent.x&&point.x>position.x)&&(point.y<extent.y&&point.y>position.y);
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
