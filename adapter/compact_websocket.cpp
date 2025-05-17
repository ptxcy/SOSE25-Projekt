#include <iostream>
#include <string>
#include <iomanip> // For std::hex, std::setw, std::setfill
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <msgpack.hpp>

// Utility function to print binary data in hex and ASCII format
void printBinaryData(const std::string& data) {
    std::cout << "MessagePack serialized data (" << data.size() << " bytes):" << std::endl;
    
    // Print hex representation
    std::cout << "Hex: ";
    for (unsigned char c : data) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
    }
    std::cout << std::dec << std::endl;
    
    // Print ASCII representation where possible
    std::cout << "ASCII: ";
    for (unsigned char c : data) {
        if (c >= 32 && c <= 126) {
            std::cout << c;
        } else {
            std::cout << '.';
        }
    }
    std::cout << std::endl;
}

std::string serialize(const std::string& str) {
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, str);
    return std::string(buffer.data(), buffer.size());
}

int main() {
    try {
        boost::asio::io_context ioc;
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};
        
        auto results = boost::asio::ip::tcp::resolver{ioc}.resolve("localhost", "8082");
        auto ep = boost::asio::connect(ws.next_layer(), results);
        ws.handshake("localhost:" + std::to_string(ep.port()), "/msgpack");
        
        std::string message = "Hello, MessagePack!";
        ws.binary(true);
        std::string serializedMessage = serialize(message);
        ws.write(boost::asio::buffer(serializedMessage));
        
        // Print the serialized message in both hex and ASCII
        printBinaryData(serializedMessage);
        
        ws.close(boost::beast::websocket::close_code::normal);
        std::cout << "Message sent successfully" << std::endl;
    } 
    catch(std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
