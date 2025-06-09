#include "server_update.h"

#ifdef FEAT_MULTIPLAYER


/**
 *	register server update procedure
 *	\param ssys: data for starsystem representation, this routine will manipulate planetary positions
 */
ServerUpdate::ServerUpdate(StarSystem* ssys)
	: m_SSys(ssys)
{
	g_Wheel.call(UpdateRoutine{ &ServerUpdate::_update,(void*)this });
}

/**
 *	interpret server messages as client data updates
 */
void ServerUpdate::update()
{
	if (!g_Websocket.server_messages.size()) return;
	ServerMessage msg = g_Websocket.receive_message();
	for (u8 i=0;i<msg.request_data.game_objects.planets.size();i++)
	{
		Planet& __Planet = msg.request_data.game_objects.planets[i];
		m_SSys->planets[i].offset = vec3(__Planet.position.x,__Planet.position.y,__Planet.position.z)*10.f;
	}
}


#endif
