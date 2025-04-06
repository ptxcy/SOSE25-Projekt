#ifndef CORE_TOOLBOX_HEADER
#define CORE_TOOLBOX_HEADER


// basics
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <chrono>

// ogl
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


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

// basic stringamagickgg
typedef std::string string;


// constants
constexpr f64 MATH_PI = 3.141592653;
constexpr f64 MATH_E = 2.7182818284;


// ----------------------------------------------------------------------------------------------------
// logger
#ifdef DEBUG

// text colour
constexpr const char* LOG_WHITE = "\e[0;30m";
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
#define COMM_MSG_FALLBACK(col,...)
#define COMM_LOG_FALLBACK(...)
#define COMM_SCC_FALLBACK(...)
#define COMM_ERR_FALLBACK(...)

#endif


#endif
