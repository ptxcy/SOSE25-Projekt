#ifndef CORE_WEBSOCKET_HEADER
#define CORE_WEBSOCKET_HEADER


#include "base.h"


#ifdef FEAT_MULTIPLAYER
#include <cpr/cpr.h>
//#include "../adapter/definition.h"
#include "../adapter/pong_adapter.h"


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
	LobbyStatus open_lobby(string& lobby_name,string& lobby_password,string& jwt_token,bool create);

private:
	string m_Addr;
};


class Websocket
{
public:
	Websocket() {  }
	void connect(string host,string port_ad,string port_ws,string name,string pass,string lnom,
				 string lpass,bool create);
	ServerMessage receive_message();
	void send_message(ClientMessage msg);
	void exit();

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
	bool state_update = false;
	std::queue<ClientMessage> client_messages;
	std::mutex mutex_server_state;
	std::mutex mutex_client_messages;

private:
	std::thread m_HandleWebsocketDownload;
	std::thread m_HandleWebsocketUpload;
	bool connected = false;
};


#ifdef FEAT_MULTIPLAYER
inline Websocket g_Websocket;
#endif


#endif
#endif
