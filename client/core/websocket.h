#ifndef CORE_WEBSOCKET_HEADER
#define CORE_WEBSOCKET_HEADER


#include "base.h"


#ifdef FEAT_MULTIPLAYER
#include <cpr/cpr.h>
#include "../adapter/definition.h"


typedef boost::beast::websocket::stream<boost::asio::ip::tcp::socket> socket_stream;


enum LobbyStatus
{
	LOBBY_UNCONNECTED,
	LOBBY_NOT_FOUND,
	LOBBY_USER_REFUSED,
	LOBBY_CONNECTED
};


class HTTPAdapter
{
public:
	HTTPAdapter(string& host,string& port);
	bool create_user(string& username,string& password);
	string authenticate_on_server(string& username,string& password);
	LobbyStatus open_lobby(string& lobby_name,string& jwt_token,bool create);

private:
	string m_Addr;
};


class Websocket
{
public:
	Websocket() {  }
	void connect(string host,string port_ad,string port_ws,string name,string pass,string lnom,bool create);

#ifdef PROJECT_PONG
	GameObject receive_message();
#elif
	ServerMessage receive_message();
#endif
	void send_message(ClientMessage msg);
	void exit();
	// FIXME project specifics do NOT belong inside engine code! add features accordingly

public:

	// system
	boost::asio::io_context ioc;
	socket_stream ws{ioc};
	bool running = true;

	// status
	string username;
	LobbyStatus lobby_status = LOBBY_UNCONNECTED;

	// messages
	ServerMessage server_state;
#ifdef PROJECT_PONG
	GameObject game_objects;
	char* raw_data;
	size_t data_size;
	msgpack::object_handle oh;
	msgpack::object_handle ohb;
	msgpack::zone zone;
	msgpack::zone zoneb;
	ThreadSignal parsing_signal;

#endif
	bool state_update = false;
	std::queue<ClientMessage> client_messages;
	std::mutex mutex_server_state;
	std::mutex mutex_client_messages;
	std::mutex mutex_msgdata_raw;

private:
	std::thread m_HandleWebsocketDownload;
	std::thread m_HandleWebsocketUpload;
	std::thread m_HandleWebsocketParsing;
};


#ifdef FEAT_MULTIPLAYER
inline Websocket g_Websocket;
#endif


#endif
#endif
