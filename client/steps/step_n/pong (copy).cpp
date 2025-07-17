#ifdef PROJECT_PONG
#include "pong.h"


/**
 *	pong setup
 */
Pong::Pong(Font* font,string name)
{
}

vec3 ctvec(Coordinate& c) { return vec3(c.x,c.y,c.z); }

/**
 *	update pong game
 */
void Pong::update()
{
	// get server updates
	if (!g_Websocket.state_update)
	{
		for (u32 i=1;i<PONG_BALL_PHYSICAL_COUNT;i++) m_BallIndices[i].position += m_BallMomentum[i];
		for (u32 i=0;i<PONG_LIGHTING_POINTLIGHTS;i++) m_BulbIndices[i].position += m_BulbMomentum[i];
		return;
	}
	GameObject __GObj = g_Websocket.receive_message();
}


#endif
