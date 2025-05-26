#ifndef CORE_WEBSOCKET_HEADER
#define CORE_WEBSOCKET_HEADER


#include "base.h"
#include "../adapter/definition.h"


typedef boost::beast::websocket::stream<boost::asio::ip::tcp::socket> socket_stream;


class Websocket
{
public:
	Websocket(string host="localhost",string port="8082");
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


#ifdef FEAT_MULTIPLAYER
inline Websocket g_Websocket = Websocket();
#endif


#endif
