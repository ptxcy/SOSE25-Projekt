#include <iostream>
#include <string>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <msgpack.hpp>

// Simple WebSocket client that sends MessagePack formatted data
int main()
{
    try
    {
        // Set up IO context
        boost::asio::io_context ioc;
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};

        std::cout << "Connecting to WebSocket server at localhost:8082/msgpack..." << std::endl;

        // Resolve and connect
        auto results = boost::asio::ip::tcp::resolver{ioc}.resolve("localhost", "8082");
        auto ep = boost::asio::connect(ws.next_layer(), results);
        ws.handshake("localhost:" + std::to_string(ep.port()), "/msgpack");

        // Set binary mode for MessagePack data
        ws.binary(true);

        // Create MessagePack buffer
        msgpack::sbuffer buffer;
        msgpack::packer<msgpack::sbuffer> packer(buffer);

        // Create a simple message structure
        packer.pack_map(1);
        packer.pack("message");
        packer.pack("Hello from C++ WebSocket client with MessagePack!");

        std::cout << "Sending message to server..." << std::endl;

        // Send the message
        ws.write(boost::asio::buffer(buffer.data(), buffer.size()));

        std::cout << "Message sent! Waiting for response..." << std::endl;

        // Create buffer for response
        boost::beast::flat_buffer response;

        // Read response
        ws.read(response);

        // Output response details
        std::cout << "Received response of " << response.size() << " bytes" << std::endl;

        // Close connection
        ws.close(boost::beast::websocket::close_code::normal);
        std::cout << "Connection closed successfully." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
