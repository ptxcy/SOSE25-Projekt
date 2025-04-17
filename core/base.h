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
static inline void ctlog(f64& delta)
{
	delta = (std::chrono::steady_clock::now()-log_delta).count()*.000001;
	printf("%s",LOG_TIMING[(u8)std::min(delta/LOG_FPS_ALERT,2.)]);
}
static inline void produce_timestamp(bool padding=true)
{
	f64 delta;
	ctlog(delta);
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
// TODO this is entirely different on windows and mac, this needs to be ported or mended towards lower systems

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
