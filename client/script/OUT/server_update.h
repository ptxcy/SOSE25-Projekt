#ifndef SCRIPT_SERVERUPDATE_HEADER
#define SCRIPT_SERVERUPDATE_HEADER

#include "../core/config.h"

#ifdef FEAT_MULTIPLAYER

#include "../core/websocket.h"
#include "starsystem.h"
#include "flotilla.h"


class ServerUpdate
{
public:
	ServerUpdate(StarSystem* ssys,Flotilla* flt);
	static inline void _update(void* su) { ServerUpdate* p = (ServerUpdate*)su; p->update(); }
	void update();

private:
	StarSystem* m_SSys;
	Flotilla* m_Flotilla;
};


#endif
#endif
