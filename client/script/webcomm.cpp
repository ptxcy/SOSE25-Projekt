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
 *	request dummy spawn
 *	\param dummy: dummy string
 */
void Request::spawn_dummy(string dummy)
{
	ClientMessage __Msg = _create_message();
	__Msg.request_data.connect = std::optional<string>(dummy);
	g_Websocket.send_message(__Msg);
}


#endif
