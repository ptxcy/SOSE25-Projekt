#include "webcomm.h"

#ifdef FEAT_MULTIPLAYER

#ifdef PROJECT_PONG

/**
 *	helper to create a message template for any request communication
 *	\returns message template
 */
ClientMessage _create_message()
{
	return {
		.username = g_Websocket.username
	};
}

/**
 *	request connection
 */
void Request::connect(string lobby)
{
	ClientMessage __Msg = _create_message();
	__Msg.request_data.connect = true;
	__Msg.request_data.lobby = std::optional(lobby);
	g_Websocket.send_message(__Msg);
}

/**
 *	transmit player movement
 *	\param dir: direction the player pedal moves towards
 */
void Request::player_movement(s8 dir)
{
	ClientMessage __Msg = _create_message();
	__Msg.request_data.move_to = dir;
	g_Websocket.send_message(__Msg);
}


#endif
#ifdef PROJECT_SPACER


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
 *	request a spaceship to spawn
 *	\param pos: position in coordinate system
 */
void Request::spawn_spaceship(vec3 pos)
{
	ClientMessage __Msg = _create_message();
	__Msg.request_data.spawn_spaceship = std::optional<Coordinate>(Coordinate{ 0,0,0 });
	g_Websocket.send_message(__Msg);
}

/**
 *	set target for spaceship to make it fly there
 *	\param id: spaceship id
 *	\param planet: target planet id
 */
void Request::set_spaceship_target(u64 id,u64 planet)
{
	ClientMessage __Msg = _create_message();
	__Msg.request_data.set_spaceship_target = std::optional<SetSpaceshipTarget>(SetSpaceshipTarget{ id,planet });
	g_Websocket.send_message(__Msg);
}


#endif
#endif
