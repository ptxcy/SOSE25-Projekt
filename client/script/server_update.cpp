#include "server_update.h"

#ifdef FEAT_MULTIPLAYER


/**
 *	TODO
 */
ServerUpdate::ServerUpdate(StarSystem* ssys)
	: m_SSys(ssys)
{
	g_Wheel.call(UpdateRoutine{ &ServerUpdate::_update,(void*)this });
}

/**
 *	TODO
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
