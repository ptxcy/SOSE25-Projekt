#include "webcomm.h"

#ifdef FEAT_MULTIPLAYER


/**
 *	helper to create a message template for any request communication
 *	\returns message template
 */
ClientMessage _create_message()
{
	return {
		.request_info = {  },
		.username = g_Websocket.username
	};
}

/**
 *	request connection
 */
void Request::connect()
{
	ClientMessage __Msg = _create_message();
	__Msg.request_data.connect = std::optional<string>(g_Websocket.username);
	g_Websocket.send_message(__Msg);
}

/**
 *	set updates per second for calculation unit update
 *	\param fps: updates per second
 */
void Request::set_fps(f64 fps)
{
	ClientMessage __Msg = _create_message();
	__Msg.request_data.set_client_fps = fps;
	g_Websocket.send_message(__Msg);
}

/**
 *	request dummy spawn
 *	\param dummy: dummy string
 */
void Request::spawn_dummy(string dummy)
{
	ClientMessage __Msg = _create_message();
	__Msg.request_data.connect = std::optional<string>(dummy);
	g_Websocket.send_message(__Msg);
}

/**
 *	TODO
 */
void Request::spawn_spaceship(vec3 pos)
{
	ClientMessage __Msg = _create_message();
	__Msg.request_data.spawn_spaceship = std::optional<Coordinate>(Coordinate{ 0,0,0 });
	g_Websocket.send_message(__Msg);
}

/**
 *	TODO
 */
void Request::set_spaceship_target(u64 id,u64 planet)
{
	ClientMessage __Msg = _create_message();
	__Msg.request_data.set_spaceship_target = std::optional<SetSpaceshipTarget>(SetSpaceshipTarget{ id,planet });
	g_Websocket.send_message(__Msg);
}


#endif
