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
	: m_Size(size/MEM_WIDTH+1)
{
	m_Data = (__system_word*)malloc(m_Size*sizeof(__system_word));
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
	cv.wait(lock,[this]{ return !semaphore; });
}

/**
 *	signal all threads listening to continue with procedure
 *	\param force: (default=false) force proceeding regardless of semaphore state
 */
void ThreadSignal::proceed(bool force)
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		semaphore--;
		semaphore *= !force;
	}
	cv.notify_one();
}

/**
 *	signal subprocess to vanish
 */
void ThreadSignal::exit()
{
	running = false;
	proceed(true);
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


// ----------------------------------------------------------------------------------------------------
// Camera

/**
 *	create 3D camera
 *	\param pos: starting camera position
 *	\param tgt: starting camera view focus target
 *	\param width: screen resolution width, can also be downscaled to ratio
 *	\param height: screen resolution height, can also be downscaled to ratio
 *	\param fov: field of view in degrees
 */
Camera3D::Camera3D(vec3 pos,vec3 tgt,f32 width,f32 height,f32 fov)
	: position(pos),target(tgt)
{
	update();
	proj = glm::perspective(glm::radians(fov),width/height,.1f,1000.f);
}

/**
 *	update camera view matrix based on position and target parameters
 */
void Camera3D::update()
{
	view = glm::lookAt(position,target,vec3(0,0,1));
}
