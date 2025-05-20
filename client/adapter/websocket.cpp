#include "websocket.h"


// Parse server response and add to queue
bool parse_server_response(const char *data, size_t size)
{
    try
    {
        // Create a msgpack zone for allocation
        msgpack::zone zone;
        msgpack::object obj = msgpack::unpack(data, size, nullptr, &zone).get();
        std::cout << "Received MessagePack object: " << obj << std::endl;

        // Try to convert to our ServerMessage structure
        ServerMessage message;
        obj.convert(message);
        serverToClientMessage.push(message);

		// logging confirmation
        std::cout << "Successfully parsed server response and added to queue." << std::endl;
        std::cout << "Queue size: " << serverToClientMessage.size() << std::endl;
        std::cout << "Target User ID: " << message.request_data.target_user_id << std::endl;
        std::cout << "Number of dummies: " << message.request_data.game_objects.dummies.size() << std::endl;
        return true;
    }

	// error communication
    catch (const std::exception& e)
    {
        std::cerr << "Error parsing server response: " << e.what() << std::endl;
        return false;
    }
}

// Function to start the websocket adapter - can be called from other files
void startWebsocketAdapter()
{
    try
    {
        std::cout << "Connecting to WebSocket server at localhost:8082/msgpack..." << std::endl;
        boost::asio::io_context ioc;
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};
        auto results = boost::asio::ip::tcp::resolver{ioc}.resolve("localhost", "8082");
        auto ep = boost::asio::connect(ws.next_layer(), results);
        ws.handshake("localhost:" + std::to_string(ep.port()), "/msgpack");
        ws.binary(true);
        std::cout << "Connected to server successfully!" << std::endl;

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
    catch (std::exception const &e) { std::cerr << "WebSocket error: " << e.what() << std::endl; }
}

// Main function to run the WebSocket client
int main()
{
	ClientMessage msg = {
		.request_info = {},
		.request_data = { .spawn_dummy = std::optional<std::string>("ich bin 1 gummibaer") },
	};
	clientToServerMessage.push(msg);
	startWebsocketAdapter();
}
