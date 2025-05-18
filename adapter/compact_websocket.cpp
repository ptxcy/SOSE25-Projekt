#include <iostream>
#include <string>
#include <iomanip> // For std::hex, std::setw, std::setfill
#include <chrono>
#include <variant>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <msgpack.hpp>

// Basic structures needed for the message format

// Coordinate structure
struct Coordinate {
    double x;
    double y;
    double z;

    MSGPACK_DEFINE(x, y, z);
};

// Client information structure (simplified - using default values)
struct ClientInfo {
    double sent_time = 0.0;

    MSGPACK_DEFINE(sent_time);
};

// Authproxy information structure (simplified - using default values)
struct AuthproxyInfo {
    double sent_time = 0.0;

    MSGPACK_DEFINE(sent_time);
};

// Request synchronization information (simplified - using default values)
struct RequestSyncInfo {
    double sent_time = 0.0;

    MSGPACK_DEFINE(sent_time);
};

// Calculation unit information (simplified - using default values)
struct CalculationUnitInfo {
    double sent_time = 0.0;

    MSGPACK_DEFINE(sent_time);
};

// Request information structure with default values
struct RequestInfo {
    ClientInfo client;
    AuthproxyInfo authproxy;
    RequestSyncInfo request_sync;
    CalculationUnitInfo calculation_unit;

    MSGPACK_DEFINE(client, authproxy, request_sync, calculation_unit);
};

// Message structures matching Rust enum variants
struct SpawnDummy {
    std::string id;

    MSGPACK_DEFINE(id);
};

struct SetClientFPS {
    double fps;

    MSGPACK_DEFINE(fps);
};

struct DummySetVelocity {
    std::string id;
    Coordinate position;

    MSGPACK_DEFINE(id, position);
};

// Helper function to create RequestInfo with default values (all zeroed)
RequestInfo createDefaultRequestInfo() {
    return RequestInfo{};
}

// Helper function to serialize SetClientFPS message with RequestInfo
void serialize_set_client_fps(msgpack::sbuffer& buffer, double fps) {
    // Start packing the message
    msgpack::packer<msgpack::sbuffer> packer(buffer);

    // Format in Rust is:
    // pub struct ClientMessage { request_info: RequestInfo, request_data: ClientRequest }
    // We need to pack this as a map with two fields
    packer.pack_map(2);

    // Field 1: request_info
    packer.pack("request_info");

    // Pack RequestInfo as a map with 4 fields
    packer.pack_map(4);

    // Field 1.1: client
    packer.pack("client");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);

    // Field 1.2: authproxy
    packer.pack("authproxy");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);

    // Field 1.3: request_sync
    packer.pack("request_sync");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);

    // Field 1.4: calculation_unit
    packer.pack("calculation_unit");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);

    // Field 2: request_data (an enum variant)
    packer.pack("request_data");

    // For Rust enum ClientRequest::SetClientFPS, we need to pack as:
    // { "SetClientFPS": fps_value }
    packer.pack_map(1);
    packer.pack("SetClientFPS");
    packer.pack(fps);
}

// Refactored helper function to pack RequestInfo with all zeroes
void pack_default_request_info(msgpack::packer<msgpack::sbuffer>& packer) {
    // Pack RequestInfo as a map with 4 fields
    packer.pack_map(4);

    // Field 1: client
    packer.pack("client");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);

    // Field 2: authproxy
    packer.pack("authproxy");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);

    // Field 3: request_sync
    packer.pack("request_sync");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);

    // Field 4: calculation_unit
    packer.pack("calculation_unit");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);
}

// Helper function to serialize SpawnDummy message with RequestInfo
void serialize_spawn_dummy(msgpack::sbuffer& buffer, const std::string& dummy_id) {
    // Start packing the message
    msgpack::packer<msgpack::sbuffer> packer(buffer);

    // Format in Rust is:
    // pub struct ClientMessage { request_info: RequestInfo, request_data: ClientRequest }
    // We need to pack this as a map with two fields
    packer.pack_map(2);

    // Field 1: request_info
    packer.pack("request_info");
    pack_default_request_info(packer);

    // Field 2: request_data (an enum variant)
    packer.pack("request_data");

    // For Rust enum ClientRequest::SpawnDummy, we need to pack as:
    // { "SpawnDummy": { "id": "..." } }
    packer.pack_map(1);
    packer.pack("SpawnDummy");
    packer.pack_map(1);
    packer.pack("id");
    packer.pack(dummy_id);
}

// Helper function to serialize DummySetVelocity message
void serialize_dummy_set_velocity(msgpack::sbuffer& buffer, const std::string& dummy_id,
                                  double x, double y, double z) {
    // Start packing the message
    msgpack::packer<msgpack::sbuffer> packer(buffer);

    // Pack the client message structure
    packer.pack_map(2);

    // Field 1: request_info
    packer.pack("request_info");
    pack_default_request_info(packer);

    // Field 2: request_data (an enum variant)
    packer.pack("request_data");

    // For Rust enum ClientRequest::DummySetVelocity, we need to pack as:
    // { "DummySetVelocity": { "id": "...", "position": { "x": x, "y": y, "z": z } } }
    packer.pack_map(1);
    packer.pack("DummySetVelocity");

    // Pack the inner map with id and position
    packer.pack_map(2);
    packer.pack("id");
    packer.pack(dummy_id);
    packer.pack("position");

    // Pack the coordinate
    packer.pack_map(3);
    packer.pack("x");
    packer.pack(x);
    packer.pack("y");
    packer.pack(y);
    packer.pack("z");
    packer.pack(z);
}

// Helper function to print hex dump of binary data
void print_hex_dump(const char* data, size_t size) {
    std::cout << "Hex dump of MessagePack data: ";
    for (size_t i = 0; i < size; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << (static_cast<int>(data[i]) & 0xFF) << " ";
    }
    std::cout << std::dec << std::endl;
}

int main() {
    try {
        std::cout << "WebSocket Client - MessagePack Test" << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        // Initialize Boost.Asio
        boost::asio::io_context ioc;
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};

        // Connect to server
        std::cout << "Connecting to localhost:8082/msgpack..." << std::endl;
        auto results = boost::asio::ip::tcp::resolver{ioc}.resolve("localhost", "8082");
        auto ep = boost::asio::connect(ws.next_layer(), results);
        ws.handshake("localhost:" + std::to_string(ep.port()), "/msgpack");
        std::cout << "Connected successfully!" << std::endl;

        // Set binary mode for MessagePack data
        ws.binary(true);

        // User menu
        int choice;
        std::cout << "\nSelect message type to send:" << std::endl;
        std::cout << "1. SpawnDummy" << std::endl;
        std::cout << "2. SetClientFPS" << std::endl;
        std::cout << "3. DummySetVelocity" << std::endl;
        std::cout << "Choice: ";
        std::cin >> choice;

        msgpack::sbuffer buffer;

        switch (choice) {
            case 1: {
                std::string dummy_id;
                std::cout << "Enter dummy ID: ";
                std::cin.ignore(); // Clear input buffer
                std::getline(std::cin, dummy_id);

                std::cout << "Creating SpawnDummy message with ID: " << dummy_id << std::endl;
                serialize_spawn_dummy(buffer, dummy_id);
                break;
            }

            case 2: {
                double fps;
                std::cout << "Enter FPS value: ";
                std::cin >> fps;

                std::cout << "Creating SetClientFPS message with value: " << fps << std::endl;
                serialize_set_client_fps(buffer, fps);
                break;
            }

            case 3: {
                std::string dummy_id;
                double x, y, z;

                std::cout << "Enter dummy ID: ";
                std::cin.ignore(); // Clear input buffer
                std::getline(std::cin, dummy_id);

                std::cout << "Enter X coordinate: ";
                std::cin >> x;
                std::cout << "Enter Y coordinate: ";
                std::cin >> y;
                std::cout << "Enter Z coordinate: ";
                std::cin >> z;

                std::cout << "Creating DummySetVelocity message with ID: " << dummy_id
                          << " and position: (" << x << ", " << y << ", " << z << ")" << std::endl;
                serialize_dummy_set_velocity(buffer, dummy_id, x, y, z);
                break;
            }

            default:
                std::cout << "Invalid choice, defaulting to SpawnDummy" << std::endl;
                serialize_spawn_dummy(buffer, "default_dummy");
                break;
        }

        // Print debug info about the binary data we're sending
        std::cout << "Sending MessagePack binary data of size: " << buffer.size() << " bytes" << std::endl;
        print_hex_dump(buffer.data(), buffer.size());

        // Send the binary message
        ws.write(boost::asio::buffer(buffer.data(), buffer.size()));
        std::cout << "Message sent successfully" << std::endl;

        // Close the WebSocket connection
        ws.close(boost::beast::websocket::close_code::normal);
    }
    catch(std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
