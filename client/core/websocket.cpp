#include "websocket.h"


/**
 *	function to handle websocket download traffic
 *	\param c: websocket data
 *	NOTE this is meant to be executed in a subthread, so the queues are filled asynchronously
 */
void _handle_websocket_download(Websocket* c)
{
	while (c->running)
	{
		try
		{
			// read buffer
			boost::beast::flat_buffer response_buffer;
			c->ws.read(response_buffer);
			auto data = response_buffer.data();
			const char* raw_data = boost::asio::buffer_cast<const char*>(data);
			size_t data_size = data.size();

			// create a msgpack zone for allocation
			msgpack::zone zone;
			msgpack::object obj = msgpack::unpack(raw_data,data_size,nullptr,&zone).get();
			COMM_LOG("received MessagePack Object %s",(std::ostringstream()<<obj).str().c_str());

			// try to convert to our ServerMessage structure
			ServerMessage message;
			obj.convert(message);
			c->mutex_server_messages.lock();
			c->server_messages.push(message);
			COMM_LOG("Parsed Response -> ID: %s, QSize: %li",
					 message.request_data.target_user_id.c_str(),c->server_messages.size());
			c->mutex_server_messages.unlock();
		}
		catch (const std::exception& e) { COMM_ERR("parsing server response -> %s",e.what()); }
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

/**
 *	function to handle websocket upload traffic
 *	\param c: websocket data
 *	NOTE this is meant to be executed in a subthread, so the queues are filled asynchronously
 */
void _handle_websocket_upload(Websocket* c)
{
	while (c->running)
	{
		if (!c->client_messages.size()) continue;
		try
		{
			c->mutex_client_messages.lock();
			ClientMessage outMsg = c->client_messages.front();
			c->client_messages.pop();
			c->mutex_client_messages.unlock();
			msgpack::sbuffer msg_buffer;
			msgpack::pack(msg_buffer,outMsg);
			c->ws.write(boost::asio::buffer(msg_buffer.data(),msg_buffer.size()));
			c->mutex_client_messages.unlock();
		}
		catch (const std::exception& e) { COMM_ERR("sending upload -> %s",e.what()); }
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
		auto results = boost::asio::ip::tcp::resolver{ioc}.resolve(host,port);
		auto ep = boost::asio::connect(ws.next_layer(),results);
		ws.handshake(host+':'+std::to_string(ep.port()),"/msgpack");
		ws.binary(true);
		COMM_SCC("Connected to server successfully!");
		// FIXME find out if the ep.port call has merit and if not replace it by predefined parameter

		// start traffic handler
		m_HandleWebsocketDownload = std::thread(_handle_websocket_download,this);
		m_HandleWebsocketUpload = std::thread(_handle_websocket_upload,this);
		connected = true;
	}
	catch (std::exception const &e) { COMM_ERR("Connection Error: %s",e.what()); }
}
// FIXME extensive usage of try-catch statements is very slow

/**
 *	receive the next server message if possible
 *	\returns next server message
 */
ServerMessage Websocket::receive_message()
{
	mutex_server_messages.lock();
	ServerMessage msg = std::move(server_messages.front());
	server_messages.pop();
	mutex_server_messages.unlock();
	return std::move(msg);
}
// FIXME in sending and receiving like this there is a lot of copying going on. not ideal cpu usage
//		yes, compensating this with std lingo is bad practice and shows architectural misunderstandings

/**
 *	send client message
 *	\param msg: client message that will be added to the sending queue
 */
void Websocket::send_message(ClientMessage msg)
{
	mutex_client_messages.lock();
	client_messages.push(std::move(msg));
	mutex_client_messages.unlock();
}

/**
 *	close the websocket traffic processes
 */
void Websocket::exit()
{
	if (!connected) return;
	running = false;
	m_HandleWebsocketDownload.join();
	m_HandleWebsocketUpload.join();
}
