#include "server_update.h"

#ifdef FEAT_MULTIPLAYER


/**
 *	register server update procedure
 *	\param ssys: data for starsystem representation, this routine will manipulate planetary positions
 *	\param flt: data for flotilla representation, this routine will manipulate spaceship positions
 */
ServerUpdate::ServerUpdate(StarSystem* ssys,Flotilla* flt)
	: m_SSys(ssys),m_Flotilla(flt)
{
	g_Wheel.call(UpdateRoutine{ &ServerUpdate::_update,(void*)this });
}

/**
 *	interpret server messages as client data updates
 */
void ServerUpdate::update()
{
	if (!g_Websocket.state_update) return;
	ServerMessage msg = g_Websocket.receive_message();

	// planetary positions
	u32 i = 0;
	m_SSys->planets.resize(msg.request_data.game_objects.planets.size());
	for (Planet& p_Planet : msg.request_data.game_objects.planets)
	{
		if (i<8) m_SSys->planets[i].texture = *m_SSys->m_PlanetTextures[i];
		else m_SSys->planets[i].texture = *m_SSys->m_PlanetTextures[0];
		m_SSys->planets[i].offset
			= vec3(p_Planet.position.x,p_Planet.position.y,p_Planet.position.z)*STARSYS_DISTANCE_SCALE;
		i++;
	}

	// fleet update
	i = 0;
	m_Flotilla->fleet.clear();
	m_Flotilla->spaceships.resize(msg.request_data.game_objects.spaceships.size());
	for (auto& it : msg.request_data.game_objects.spaceships)
	{
		Spaceship& p_Spaceship = it.second;
		if (!p_Spaceship.owner.compare(g_Websocket.username))
			m_Flotilla->fleet.insert({ p_Spaceship.id,p_Spaceship});
		m_Flotilla->spaceships[i].offset
			= vec3(p_Spaceship.position.x,p_Spaceship.position.y,p_Spaceship.position.z)*STARSYS_DISTANCE_SCALE;
		i++;
	}
}


#endif
