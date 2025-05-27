#ifndef CORE_WEBSOCKET_HEADER
#define CORE_WEBSOCKET_HEADER


#include "base.h"
#include <cpr/cpr.h>
#include "../adapter/definition.h"


typedef boost::beast::websocket::stream<boost::asio::ip::tcp::socket> socket_stream;


class HTTPAdapter
{
public:
	HTTPAdapter(string& host,string& port);
	bool create_user(string& username,string& password);
	string authenticate_on_server(string& username,string& password);
	void open_lobby(string& lobby_name,string& lobby_password,string& jwt_token,bool create);

private:
	string m_Addr;
};


class Websocket
{
public:
	Websocket(string host,string port_ad,string port_ws,string name,string pass,string lnom,
			  string lpass,bool create);
	ServerMessage receive_message();
	void send_message(ClientMessage msg);
	void exit();

public:
	std::queue<ServerMessage> server_messages;
	std::queue<ClientMessage> client_messages;
	std::mutex mutex_server_messages;
	std::mutex mutex_client_messages;
	boost::asio::io_context ioc;
	socket_stream ws{ioc};
	bool running = true;

private:
	std::thread m_HandleWebsocketDownload;
	std::thread m_HandleWebsocketUpload;
	bool connected = false;
};


#endif
