#ifndef SCRIPT_WEBCOMM_HEADER
#define SCRIPT_WEBCOMM_HEADER


#include "../core/config.h"

#ifdef FEAT_MULTIPLAYER
#include "../core/websocket.h"


class Request
{
public:

#ifdef PROJECT_PONG
	static void connect(string lobby);
	static void player_movement(s8 dir);
#endif

#ifdef PROJECT_SPACER
	static void connect();
	static void set_fps(f64 fps);
	static void spawn_dummy(string dummy);
	static void spawn_spaceship(vec3 pos);
	static void set_spaceship_target(u64 id,u64 planet);
#endif
};


#endif
#endif
