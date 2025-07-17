#ifdef PROJECT_PONG
#include "pong.h"


/**
 *	pong setup
 */
Pong::Pong(Font* font,string name)
{
	// camera setup
	g_Camera.distance = 100.f;
	g_Camera.pitch = glm::radians(45.f);

	// connection to server
	string lobby_name = "pong-anaconda";
	g_Websocket.connect(NETWORK_HOST,NETWORK_PORT_ADAPTER,NETWORK_PORT_WEBSOCKET,
						name,"wilson",lobby_name,!strcmp(name.c_str(),"owen"));

	COMM_LOG("sleeping for 300ms so socket is ready");
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	Request::connect(lobby_name);
	COMM_AWT("sleeping so lobby is registered");
	std::this_thread::sleep_for(std::chrono::milliseconds(NETWORK_CONNECTION_STALL));
	COMM_CNF();

	g_Wheel.call(UpdateRoutine{ &Pong::_update,(void*)this });
}

vec3 ctvec(Coordinate& c) { return vec3(c.x,c.y,c.z); }

/**
 *	update pong game
 */
void Pong::update()
{
	// player input
	// TODO input

	// get server updates
	if (!g_Websocket.state_update) return;
	GameObject __GObj = g_Websocket.receive_message();
	// TODO server update
}


#endif
