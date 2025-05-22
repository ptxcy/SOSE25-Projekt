#include "websocket.h"


/**
 *	Parse server response and add to queue
 *	\param data: raw response data
 *	\param size: length of received response data
 */
void parse_server_response(const char* data, size_t size)
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
		serverToClientMessage.push(message);

		COMM_LOG("Parsed Response: ID: %s, QSize: %li",
				 message.request_data.target_user_id.c_str(),serverToClientMessage.size());
	}

	catch (const std::exception& e) { COMM_ERR("Error parsing server response: %s",e.what()); }
}

/**
 *	Function to start the websocket adapter
 *	\param host: (default=localhost) websocket host
 *	\param port: (default=8082) websocket port
 *	NOTE this is meant to be executed in a subthread, so the queues are filled asynchronously
 */
void startWebsocketAdapter(string host,string port)
{
	try
	{
		COMM_LOG("Connecting to WebSocket server at %s:%s/msgpack...",host.c_str(),port.c_str());
		boost::asio::io_context ioc;
		boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};
		auto results = boost::asio::ip::tcp::resolver{ioc}.resolve(host,port);
		auto ep = boost::asio::connect(ws.next_layer(),results);
		ws.handshake(host+':'+std::to_string(ep.port()),"/msgpack");
		ws.binary(true);
		COMM_SCC("Connected to server successfully!");
		// FIXME find out if the ep.port call has merit and if not replace it by predefined parameter

		while (true)
		{
			// Check if there are messages to send
			if (clientToServerMessage.size())
			{
				ClientMessage outMsg = clientToServerMessage.front();
				clientToServerMessage.pop();
				msgpack::sbuffer msg_buffer;
				msgpack::pack(msg_buffer,outMsg);
				ws.write(boost::asio::buffer(msg_buffer.data(),msg_buffer.size()));
			}

			// Create a buffer for the response
			boost::beast::flat_buffer response_buffer;
			ws.read(response_buffer);
			auto data = response_buffer.data();
			const char* raw_data = boost::asio::buffer_cast<const char*>(data);
			size_t data_size = data.size();
			parse_server_response(raw_data, data_size);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	catch (std::exception const &e) { COMM_ERR("WebSocket error: %s",e.what()); }
}
// TODO split up this handler into upload & download
