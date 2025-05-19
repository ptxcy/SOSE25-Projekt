#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
#include <variant>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <msgpack.hpp>

// Basic structures needed for the message format

// Coordinate structure
struct Coordinate
{
    double x;
    double y;
    double z;

    MSGPACK_DEFINE(x, y, z);
};

// Client information structure (simplified - using default values)
struct ClientInfo
{
    double sent_time = 0.0;

    MSGPACK_DEFINE(sent_time);
};

// Authproxy information structure (simplified - using default values)
struct AuthproxyInfo
{
    double sent_time = 0.0;

    MSGPACK_DEFINE(sent_time);
};

// Request synchronization information (simplified - using default values)
struct RequestSyncInfo
{
    double sent_time = 0.0;

    MSGPACK_DEFINE(sent_time);
};

// Calculation unit information (simplified - using default values)
struct CalculationUnitInfo
{
    double sent_time = 0.0;

    MSGPACK_DEFINE(sent_time);
};

// Request information structure with default values
struct RequestInfo
{
    ClientInfo client;
    AuthproxyInfo authproxy;
    RequestSyncInfo request_sync;
    CalculationUnitInfo calculation_unit;

    MSGPACK_DEFINE(client, authproxy, request_sync, calculation_unit);
};

// Message structures matching Rust enum variants
struct SpawnDummy
{
    std::string id;

    MSGPACK_DEFINE(id);
};

struct SetClientFPS
{
    double fps;

    MSGPACK_DEFINE(fps);
};

struct DummySetVelocity
{
    std::string id;
    Coordinate position;

    MSGPACK_DEFINE(id, position);
};

// Response structures (based on expected server responses)
struct ServerInfo
{
    double sent_time = 0.0;

    MSGPACK_DEFINE(sent_time);
};

struct ResponseInfo
{
    ClientInfo client;
    AuthproxyInfo authproxy;
    ServerInfo server;

    MSGPACK_DEFINE(client, authproxy, server);
};

// Helper function to create RequestInfo with default values (all zeroed)
RequestInfo createDefaultRequestInfo()
{
    return RequestInfo{};
}

// Refactored helper function to pack RequestInfo with all zeroes
void pack_default_request_info(msgpack::packer<msgpack::sbuffer> &packer)
{
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
void serialize_spawn_dummy(msgpack::sbuffer &buffer, const std::string &dummy_id)
{
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

// Helper function to serialize SetClientFPS message with RequestInfo
void serialize_set_client_fps(msgpack::sbuffer &buffer, double fps)
{
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

    // For Rust enum ClientRequest::SetClientFPS, we need to pack as:
    // { "SetClientFPS": fps_value }
    packer.pack_map(1);
    packer.pack("SetClientFPS");
    packer.pack(fps);
}

// Helper function to serialize DummySetVelocity message
void serialize_dummy_set_velocity(msgpack::sbuffer &buffer, const std::string &dummy_id,
                                  double x, double y, double z)
{
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
void print_hex_dump(const char *data, size_t size)
{
    std::cout << "Hex dump:" << std::endl;

    const size_t LINE_LENGTH = 16;
    for (size_t i = 0; i < size; i += LINE_LENGTH)
    {
        // Print offset
        std::cout << std::hex << std::setw(8) << std::setfill('0') << i << "  ";

        // Print hex values
        for (size_t j = 0; j < LINE_LENGTH; ++j)
        {
            if (i + j < size)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0')
                          << (static_cast<int>(data[i + j]) & 0xFF) << " ";
            }
            else
            {
                std::cout << "   "; // 3 spaces to align
            }

            // Add extra space at the middle
            if (j == 7)
            {
                std::cout << " ";
            }
        }

        // Print ASCII representation
        std::cout << " |";
        for (size_t j = 0; j < LINE_LENGTH; ++j)
        {
            if (i + j < size)
            {
                char c = data[i + j];
                if (c >= 32 && c <= 126)
                { // Printable ASCII
                    std::cout << c;
                }
                else
                {
                    std::cout << '.'; // Non-printable
                }
            }
            else
            {
                std::cout << " "; // space for alignment
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << std::dec << std::endl; // Reset to decimal output
}

// Helper function to interpret MessagePack response
void interpret_msgpack_response(const char *data, size_t size)
{
    try
    {
        // Create msgpack zone for memory allocation
        msgpack::zone zone;

        // Unpack the data
        msgpack::object obj = msgpack::unpack(data, size, nullptr, &zone).get();

        std::cout << "\n===== MSGPACK INTERPRETATION =====\n";
        std::cout << "Object type: ";

        switch (obj.type)
        {
        case msgpack::type::NIL:
            std::cout << "NIL";
            break;
        case msgpack::type::BOOLEAN:
            std::cout << "BOOLEAN";
            break;
        case msgpack::type::POSITIVE_INTEGER:
            std::cout << "POSITIVE_INTEGER";
            break;
        case msgpack::type::NEGATIVE_INTEGER:
            std::cout << "NEGATIVE_INTEGER";
            break;
        case msgpack::type::FLOAT32:
        case msgpack::type::FLOAT64:
            std::cout << "FLOAT";
            break;
        case msgpack::type::STR:
            std::cout << "STRING";
            break;
        case msgpack::type::BIN:
            std::cout << "BINARY";
            break;
        case msgpack::type::ARRAY:
            std::cout << "ARRAY";
            break;
        case msgpack::type::MAP:
            std::cout << "MAP";
            break;
        case msgpack::type::EXT:
            std::cout << "EXTENSION";
            break;
        default:
            std::cout << "UNKNOWN";
            break;
        }
        std::cout << std::endl;

        // Try to interpret the structure based on expected response format
        if (obj.type == msgpack::type::MAP)
        {
            std::cout << "Top-level structure: MAP with " << obj.via.map.size << " elements\n";

            // Pretty print the parsed structure (handling nested maps)
            std::cout << "Structure: " << obj << std::endl;

            // Check if this follows the expected ServerMessage format with response_info and response_data
            bool has_response_info = false;
            bool has_response_data = false;

            for (uint32_t i = 0; i < obj.via.map.size; ++i)
            {
                const auto &kv = obj.via.map.ptr[i];
                if (kv.key.type == msgpack::type::STR)
                {
                    std::string key(kv.key.via.str.ptr, kv.key.via.str.size);
                    if (key == "response_info")
                    {
                        has_response_info = true;
                        std::cout << "\nFound response_info field\n";
                    }
                    else if (key == "response_data")
                    {
                        has_response_data = true;
                        std::cout << "\nFound response_data field\n";
                    }
                }
            }

            if (has_response_info && has_response_data)
            {
                std::cout << "\nRecognized as ServerMessage format with both response_info and response_data\n";
            }
            else
            {
                std::cout << "\nDoes not match expected ServerMessage format completely\n";
            }
        }

        std::cout << "================================\n";
    }
    catch (std::exception &e)
    {
        std::cout << "Failed to interpret MessagePack data: " << e.what() << std::endl;
    }
}

int main()
{
    try
    {
        std::cout << "Improved WebSocket Client - MessagePack Test" << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        // Initialize Boost.Asio
        boost::asio::io_context ioc;
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};

        // Connect to server
        std::cout << "Connecting to localhost:8082/msgpack..." << std::endl;
        auto results = boost::asio::ip::tcp::resolver{ioc}.resolve("localhost", "8082");
        auto ep = boost::asio::connect(ws.next_layer(), results);

        // Set WebSocket options
        ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(
            boost::beast::role_type::client));

        // Perform the handshake
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

        switch (choice)
        {
        case 1:
        {
            std::string dummy_id;
            std::cout << "Enter dummy ID: ";
            std::cin.ignore(); // Clear input buffer
            std::getline(std::cin, dummy_id);

            std::cout << "Creating SpawnDummy message with ID: " << dummy_id << std::endl;
            serialize_spawn_dummy(buffer, dummy_id);
            break;
        }

        case 2:
        {
            double fps;
            std::cout << "Enter FPS value: ";
            std::cin >> fps;

            std::cout << "Creating SetClientFPS message with value: " << fps << std::endl;
            serialize_set_client_fps(buffer, fps);
            break;
        }

        case 3:
        {
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
        std::cout << "Sending message to server..." << std::endl;
        ws.write(boost::asio::buffer(buffer.data(), buffer.size()));
        std::cout << "Message sent successfully." << std::endl;

        std::cout << "Waiting for server response (5 second timeout)..." << std::endl;

        // Create a buffer for the response
        boost::beast::flat_buffer response_buffer;

        // Try to read a response with a timeout
        bool response_received = false;

        try
        {
            // Start a timer for timeout
            ws.set_option(boost::beast::websocket::stream_base::timeout{
                std::chrono::seconds(5), // Handshake timeout
                std::chrono::seconds(5), // Idle timeout
                false                    // Keep-alive pings
            });

            // Read a message into our buffer
            ws.read(response_buffer);
            response_received = true;

            std::cout << "\n===== RESPONSE RECEIVED =====\n";
            std::cout << "Size: " << response_buffer.data().size() << " bytes\n";

            // Print detailed hex dump
            print_hex_dump(
                static_cast<const char *>(response_buffer.data().data()),
                response_buffer.data().size());

            // Attempt to interpret the MessagePack data
            interpret_msgpack_response(
                static_cast<const char *>(response_buffer.data().data()),
                response_buffer.data().size());
        }
        catch (boost::system::system_error &e)
        {
            std::cout << "Error reading response: " << e.what() << std::endl;
            if (e.code() == boost::beast::websocket::error::closed)
            {
                std::cout << "WebSocket connection was closed by the server." << std::endl;
            }
            else if (e.code() == boost::asio::error::operation_aborted)
            {
                std::cout << "Response timeout - no data received within timeout period." << std::endl;
            }
        }
        catch (std::exception &e)
        {
            std::cout << "General error: " << e.what() << std::endl;
        }

        if (!response_received)
        {
            std::cout << "No response was received from the server." << std::endl;
        }

        // Close the WebSocket connection
        std::cout << "\nClosing connection..." << std::endl;
        try
        {
            ws.close(boost::beast::websocket::close_code::normal);
            std::cout << "Connection closed gracefully." << std::endl;
        }
        catch (std::exception &e)
        {
            std::cout << "Error while closing connection: " << e.what() << std::endl;
        }
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
