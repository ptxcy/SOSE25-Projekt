#ifndef CORE_BASE_HEADER
#define CORE_BASE_HEADER


#if INTPTR_MAX == INT64_MAX
#define __SYSTEM_64BIT
#else
#define __SYSTEM_32BIT
#endif


// basics
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

// ogl
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// math
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// texture
#ifndef STBI_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "include/stb_image.h"
#endif


// ----------------------------------------------------------------------------------------------------
// basetype definitions to n64 standard
// unsigned data
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// signed integers
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

// floating points
typedef float f32;
typedef double f64;

// vectors
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::quat quat;
typedef glm::mat4 mat4;

// basic stringamagickgg
typedef std::string string;


// constants
// basic math
constexpr f32 FRAME_RESOLUTION_X_INV = 1.f/FRAME_RESOLUTION_X;
constexpr f32 FRAME_RESOLUTION_Y_INV = 1.f/FRAME_RESOLUTION_Y;
constexpr f32 MATH_CARTESIAN_XRANGE = 1280.f;
constexpr f32 MATH_CARTESIAN_YRANGE = 720.f;
constexpr f32 MATH_CARTESIAN_XRANGE_INV = 1.f/MATH_CARTESIAN_XRANGE;
constexpr f32 MATH_CARTESIAN_YRANGE_INV = 1.f/MATH_CARTESIAN_YRANGE;
constexpr f64 MATH_PI = 3.141592653;
constexpr f64 MATH_E = 2.7182818284;
constexpr f64 MATH_CONVERSION_MS = .000001;

// memory layout based on build target
typedef
#ifdef __SYSTEM_64BIT
	u64
#else
	u32
#endif
	__system_word;
constexpr u8 MEM_WIDTH = sizeof(__system_word)*8;
constexpr u8 MEM_SHIFT = log2(MEM_WIDTH);
constexpr __system_word MEM_MASK = MEM_WIDTH-1;


// ----------------------------------------------------------------------------------------------------
// logger
#ifdef DEBUG

// text colour
constexpr const char* LOG_WHITE = "\e[1;30m";
constexpr const char* LOG_RED = "\e[1;31m";
constexpr const char* LOG_GREEN = "\e[1;32m";
constexpr const char* LOG_YELLOW = "\e[1;33m";
constexpr const char* LOG_BLUE = "\e[1;34m";
constexpr const char* LOG_PURPLE = "\e[1;35m";
constexpr const char* LOG_CYAN = "\e[1;36m";
constexpr const char* LOG_GREY = "\e[1;90m";
constexpr const char* LOG_CLEAR = "\e[0;39m";

// time records
constexpr f64 LOG_FPS_ALERT = 16.6;
constexpr const char* LOG_TIMING[] = { LOG_GREY,LOG_YELLOW,LOG_RED };
inline std::chrono::steady_clock::time_point log_delta = std::chrono::steady_clock::now();
static inline void reset_timestamp() { log_delta = std::chrono::steady_clock::now(); }
static inline void produce_timestamp(bool padding=true)
{
	f64 delta = (std::chrono::steady_clock::now()-log_delta).count()*MATH_CONVERSION_MS;
	printf("%s",LOG_TIMING[(u8)std::min(delta/LOG_FPS_ALERT,2.)]);
	printf((padding) ? "%12fms%s" : "%fms%s\n",delta,LOG_CLEAR);
	reset_timestamp();
}

// logger features basic
#define COMM_AWT(...) reset_timestamp(),printf(__VA_ARGS__),printf("... ");
#define COMM_CNF() printf("%sdone%s in ",LOG_GREEN,LOG_GREY),produce_timestamp(false);
#define COMM_MSG(col,...) produce_timestamp(),printf(" | %s",col),printf(__VA_ARGS__),printf("%s\n",LOG_CLEAR);
#define COMM_LOG(...) COMM_MSG(LOG_WHITE,__VA_ARGS__);
#define COMM_SCC(...) COMM_MSG(LOG_GREEN,__VA_ARGS__);
#define COMM_ERR(...) printf("%serror: ",LOG_RED),printf(__VA_ARGS__),printf("%s\n",LOG_CLEAR);

// logger features conditional
#define COMM_MSG_COND(cnd,col,...) if (cnd) { COMM_MSG(col,__VA_ARGS); }
#define COMM_LOG_COND(cnd,...) if (cnd) { COMM_LOG(__VA_ARGS__); }
#define COMM_SCC_COND(cnd,...) if (cnd) { COMM_SCC(__VA_ARGS__); }
#define COMM_ERR_COND(cnd,...) if (cnd) { COMM_ERR(__VA_ARGS__); }
#define COMM_MSG_FALLBACK(col,...) else { COMM_MSG(col,__VA_ARGS__); }
#define COMM_LOG_FALLBACK(...) else { COMM_LOG(__VA_ARGS__); }
#define COMM_SCC_FALLBACK(...) else { COMM_SCC(__VA_ARGS__); }
#define COMM_ERR_FALLBACK(...) else { COMM_ERR(__VA_ARGS__); }
// TODO colouring is entirely different on windows and mac, this needs to be ported towards lower systems
// FIXME when multithreading this system does not print all comms on some architectures, rather strange

// runtime profiler
constexpr u16 PROFILER_FRAMES_RELEVANT_AVERAGE = 300;
struct RuntimeProfilerData
{
	const char* name;
	std::chrono::steady_clock::time_point last;
	f64 measurements[PROFILER_FRAMES_RELEVANT_AVERAGE] = { 0 };
	u16 head = 0;
};
static inline void profiler_tick(RuntimeProfilerData* data)
{
	data->measurements[data->head] = (std::chrono::steady_clock::now()-data->last).count()*MATH_CONVERSION_MS;
	data->head = (data->head+1)%PROFILER_FRAMES_RELEVANT_AVERAGE;
	data->last = std::chrono::steady_clock::now();
}
static inline f64 profiler_average(RuntimeProfilerData* data)
{
	f64 sum = .0;
	for (u16 i=0;i<PROFILER_FRAMES_RELEVANT_AVERAGE;i++) sum += data->measurements[i];
	return sum/PROFILER_FRAMES_RELEVANT_AVERAGE;
}
// TODO display all profiler steps in a debug-only wheel function... essentially write profiler for debug mode!

// runtime profiler features
#define PROF_CRT(nom) { .name = nom };
#define PROF_STA(d) d.last = std::chrono::steady_clock::now();
#define PROF_STP(d) profiler_tick(&d);
#define PROF_SHW(d) printf("%sprofiler: %12fms | %s%s\n",LOG_BLUE,profiler_average(&d),d.name,LOG_CLEAR);

#else

// nulldefinition for release build
#define COMM_AWT(...)
#define COMM_CNF()
#define COMM_MSG(col,...)
#define COMM_LOG(...)
#define COMM_SCC(...)
#define COMM_ERR(...)
#define COMM_MSG_COND(cnd,col,...)
#define COMM_LOG_COND(cnd,...)
#define COMM_SCC_COND(cnd,...)
#define COMM_ERR_COND(cnd,...)
#define COMM_MSG_FALLBACK(col,...)
#define COMM_LOG_FALLBACK(...)
#define COMM_SCC_FALLBACK(...)
#define COMM_ERR_FALLBACK(...)
#define PROF_CRT(nom)
#define PROF_STA(d)
#define PROF_STP(d)
#define PROF_SHW(d)

#endif


bool check_file_exists(const char* path);


class BitwiseWords
{
public:
	BitwiseWords(size_t size);
	~BitwiseWords();

	inline bool operator[](size_t i) { return (*(m_Data+(i>>MEM_SHIFT))>>(i&MEM_MASK))&1u; }
	inline void set(size_t i) { *(m_Data+(i>>MEM_SHIFT))|=1u<<(i&MEM_MASK); }
	inline void unset(size_t i) { *(m_Data+(i>>MEM_SHIFT))&=~1u<<(i&MEM_MASK); }
	inline void reset() { memset(m_Data,0,m_Size); }

private:
	__system_word* m_Data;
	size_t m_Size;
};


template<typename T> class InPlaceArray
{
public:
	InPlaceArray(u16 size)
#ifdef DEBUG
			: m_Size(size)
#endif
		{ mem = (T*)malloc(size*sizeof(T)); }
	~InPlaceArray() { free(mem); }

	/**
	 *	get pointer to next free memory slot without segmentation and extensive range extensions
	 *	\returns pointer to optimal free memory slot
	 */
	inline T* next_free()
	{
		if (overwrites.size())
		{
			T* out = &mem[overwrites.front()];
			overwrites.pop();
			return out;
		}
		COMM_ERR_COND(m_Size<active_range+1,
					  "data registration violates maximum range, consider adjusting the creation constant");
		return &mem[active_range++];
	}

public:
	u16 active_range = 0;
	T* mem;
	std::queue<u16> overwrites;

#ifdef DEBUG
private: u16 m_Size;
#endif
};


struct ThreadSignal
{
	// utility
	void wait();
	void proceed();
	void exit();

	// data
#ifdef DEBUG
	const char* name;
#endif
	std::mutex mutex;
	std::condition_variable cv;
	bool active = false;
	bool running = true;
};


struct Rect
{
	// utility
	bool intersect(vec2 point);

	// data
	vec2 position;
	vec2 extent;
};


class CoordinateSystem2D
{
public:
	CoordinateSystem2D(f32 xaxis,f32 yaxis);

public:
	mat4 view;
	mat4 proj;
};

inline CoordinateSystem2D g_CoordinateSystem = CoordinateSystem2D(MATH_CARTESIAN_XRANGE,MATH_CARTESIAN_YRANGE);


#endif
