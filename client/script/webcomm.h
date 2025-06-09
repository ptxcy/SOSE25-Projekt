#ifndef SCRIPT_WEBCOMM_HEADER
#define SCRIPT_WEBCOMM_HEADER


#include "../core/config.h"

#ifdef FEAT_MULTIPLAYER
#include "../core/websocket.h"


class Request
{
public:
	static void connect();
	static void spawn_dummy(string dummy);
};


#endif
#endif
