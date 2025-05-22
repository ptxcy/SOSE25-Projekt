#include "websocket.h"


#ifdef FEAT_MULTIPLAYER
Websocket g_Websocket;
#endif


/**
 *	Parse server response and add to queue
 *	\param c: websocket data
 *	\param data: raw response data
 *	\param size: length of received response data
 */
void _parse_server_response(WebsocketComplex* c,const char* data, size_t size)
{
	try
	{
		// Create a msgpack zone for allocation
		msgpack::zone zone;
		msgpack::object obj = msgpack::unpack(data,size,nullptr,&zone).get();
		COMM_LOG("received MessagePack Object %s",(std::ostringstream()<<obj).str().c_str());

		// Try to convert to our ServerMessage structure
		ServerMessage message;
		obj.convert(message);
		c->m_MutexServerMessages.lock();
		c->server_messages.push(message);

		COMM_LOG("Parsed Response: ID: %s, QSize: %li",
				 message.request_data.target_user_id.c_str(),c->server_messages.size());
		c->m_MutexServerMessages.unlock();
	}

	catch (const std::exception& e) { COMM_ERR("Error parsing server response: %s",e.what()); }
}

/**
 *	function to handle websocket download traffic
 *	NOTE this is meant to be executed in a subthread, so the queues are filled asynchronously
 */
void _handle_websocket_download(WebsocketComplex* c)
{
	while (true)
	{
		boost::beast::flat_buffer response_buffer;
		c->ws.read(response_buffer);
		auto data = response_buffer.data();
		const char* raw_data = boost::asio::buffer_cast<const char*>(data);
		size_t data_size = data.size();
		_parse_server_response(c,raw_data,data_size);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
// TODO lock queue when updating
// TODO allow handler to stop updating when window is closed

/**
 *	function to handle websocket upload traffic
 *	NOTE this is meant to be executed in a subthread, so the queues are filled asynchronously
 */
void _handle_websocket_upload(WebsocketComplex* c)
{
	while (true)
	{
		c->m_MutexClientMessages.lock();
		if (c->client_messages.size())
		{
			ClientMessage outMsg = c->client_messages.front();
			c->client_messages.pop();
			c->m_MutexClientMessages.unlock();
			msgpack::sbuffer msg_buffer;
			msgpack::pack(msg_buffer,outMsg);
			c->ws.write(boost::asio::buffer(msg_buffer.data(),msg_buffer.size()));
		}
		c->m_MutexClientMessages.unlock();
	}
}


/**
 *	automatically run the necessary setup to update & process the websocket queues
 *	\param host: (default=localhost) websocket host
 *	\param port: (default=8082) websocket port
 */
Websocket::Websocket(string host,string port)
{
	try
	{
		COMM_LOG("Connecting to WebSocket server at %s:%s/msgpack...",host.c_str(),port.c_str());
		auto results = boost::asio::ip::tcp::resolver{complex.ioc}.resolve(host,port);
		auto ep = boost::asio::connect(complex.ws.next_layer(),results);
		complex.ws.handshake(host+':'+std::to_string(ep.port()),"/msgpack");
		complex.ws.binary(true);
		COMM_SCC("Connected to server successfully!");
		// FIXME find out if the ep.port call has merit and if not replace it by predefined parameter
	}
	catch (std::exception const &e) { COMM_ERR("Connection Error: %s",e.what()); }

	std::thread __HandleWebsocketDownload(_handle_websocket_download,&complex);
	std::thread __HandleWebsocketUpload(_handle_websocket_upload,&complex);
	__HandleWebsocketDownload.detach();
	__HandleWebsocketUpload.detach();
}
