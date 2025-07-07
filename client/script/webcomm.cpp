#include "webcomm.h"

#ifdef FEAT_MULTIPLAYER


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
void Request::connect()
{
	ClientMessage __Msg = _create_message();
	__Msg.request_data.connect = true;
	g_Websocket.send_message(__Msg);
}


#endif
