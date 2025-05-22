#ifndef CORE_WEBSOCKET_HEADER
#define CORE_WEBSOCKET_HEADER


#include "base.h"
#include "../adapter/definition.h"


typedef boost::beast::websocket::stream<boost::asio::ip::tcp::socket> socket_stream;

struct WebsocketComplex
{
	std::queue<ServerMessage> server_messages;
	std::queue<ClientMessage> client_messages;
	std::mutex m_MutexServerMessages;
	std::mutex m_MutexClientMessages;
	boost::asio::io_context ioc;
	socket_stream ws{ioc};
};


// voluntary websocket feature
class Websocket
{
public:
	Websocket(string host="localhost",string port="8082");

public:
	WebsocketComplex complex;
};


#ifdef FEAT_MULTIPLAYER
extern Websocket g_Websocket;
#endif


#endif
