#include <iostream>
#include <string>
#include <iomanip>
#include <chrono>
#include <variant>
#include <map>
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

    friend std::ostream &operator<<(std::ostream &os, const Coordinate &coord)
    {
        return os << "(" << coord.x << ", " << coord.y << ", " << coord.z << ")";
    }
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

// Response data structures
struct DummySpawned
{
    std::string id;
    Coordinate position;

    MSGPACK_DEFINE(id, position);
};

struct ClientFPSSet
{
    double fps;

    MSGPACK_DEFINE(fps);
};

struct DummyVelocitySet
{
    std::string id;
    Coordinate position;

    MSGPACK_DEFINE(id, position);
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

// Helper function to analyze a response_data field in MessagePack format
void analyze_response_data(const msgpack::object &obj)
{
    if (obj.type != msgpack::type::MAP || obj.via.map.size == 0)
    {
        std::cout << "Response data is not a valid map or is empty\n";
        return;
    }

    bool found_variant = false;

    // Iterate through the map looking for a known response variant
    for (uint32_t i = 0; i < obj.via.map.size; ++i)
    {
        auto &kv = obj.via.map.ptr[i];
        if (kv.key.type == msgpack::type::STR)
        {
            std::string key_str(kv.key.via.str.ptr, kv.key.via.str.size);

            std::cout << "Response variant: " << key_str << std::endl;
            found_variant = true;

            // Process different response types
            if (key_str == "DummySpawned")
            {
                try
                {
                    // Try to extract the DummySpawned data
                    if (kv.val.type == msgpack::type::MAP)
                    {
                        std::string id;
                        Coordinate position;

                        // Manually extract fields from the map
                        for (uint32_t j = 0; j < kv.val.via.map.size; ++j)
                        {
                            auto &field_kv = kv.val.via.map.ptr[j];
                            if (field_kv.key.type == msgpack::type::STR)
                            {
                                std::string field_name(field_kv.key.via.str.ptr, field_kv.key.via.str.size);

                                if (field_name == "id" && field_kv.val.type == msgpack::type::STR)
                                {
                                    id = std::string(field_kv.val.via.str.ptr, field_kv.val.via.str.size);
                                }
                                else if (field_name == "position" && field_kv.val.type == msgpack::type::MAP)
                                {
                                    // Extract position coordinates
                                    auto &pos_map = field_kv.val;
                                    for (uint32_t k = 0; k < pos_map.via.map.size; ++k)
                                    {
                                        auto &coord_kv = pos_map.via.map.ptr[k];
                                        if (coord_kv.key.type == msgpack::type::STR)
                                        {
                                            std::string coord_name(coord_kv.key.via.str.ptr, coord_kv.key.via.str.size);
                                            double value = 0.0;
                                            if (coord_kv.val.type == msgpack::type::FLOAT)
                                            {
                                                value = coord_kv.val.via.f64;
                                            }
                                            else if (coord_kv.val.type == msgpack::type::POSITIVE_INTEGER)
                                            {
                                                value = static_cast<double>(coord_kv.val.via.u64);
                                            }
                                            else if (coord_kv.val.type == msgpack::type::NEGATIVE_INTEGER)
                                            {
                                                value = static_cast<double>(coord_kv.val.via.i64);
                                            }

                                            if (coord_name == "x")
                                                position.x = value;
                                            else if (coord_name == "y")
                                                position.y = value;
                                            else if (coord_name == "z")
                                                position.z = value;
                                        }
                                    }
                                }
                            }
                        }

                        // Print out the extracted information
                        std::cout << "Dummy spawned with ID: " << id << std::endl;
                        std::cout << "Position: " << position << std::endl;
                    }
                }
                catch (std::exception &e)
                {
                    std::cout << "Error parsing DummySpawned data: " << e.what() << std::endl;
                }
            }
            else if (key_str == "ClientFPSSet")
            {
                try
                {
                    double fps = 0;
                    if (kv.val.type == msgpack::type::FLOAT)
                    {
                        fps = kv.val.via.f64;
                    }
                    else if (kv.val.type == msgpack::type::POSITIVE_INTEGER)
                    {
                        fps = static_cast<double>(kv.val.via.u64);
                    }
                    else if (kv.val.type == msgpack::type::NEGATIVE_INTEGER)
                    {
                        fps = static_cast<double>(kv.val.via.i64);
                    }

                    std::cout << "Client FPS set to: " << fps << std::endl;
                }
                catch (std::exception &e)
                {
                    std::cout << "Error parsing ClientFPSSet data: " << e.what() << std::endl;
                }
            }
            else if (key_str == "DummyVelocitySet")
            {
                try
                {
                    // Try to extract the DummyVelocitySet data
                    if (kv.val.type == msgpack::type::MAP)
                    {
                        std::string id;
                        Coordinate position;

                        // Similar extraction logic as for DummySpawned
                        for (uint32_t j = 0; j < kv.val.via.map.size; ++j)
                        {
                            auto &field_kv = kv.val.via.map.ptr[j];
                            if (field_kv.key.type == msgpack::type::STR)
                            {
                                std::string field_name(field_kv.key.via.str.ptr, field_kv.key.via.str.size);

                                if (field_name == "id" && field_kv.val.type == msgpack::type::STR)
                                {
                                    id = std::string(field_kv.val.via.str.ptr, field_kv.val.via.str.size);
                                }
                                else if (field_name == "position" && field_kv.val.type == msgpack::type::MAP)
                                {
                                    // Extract position coordinates
                                    auto &pos_map = field_kv.val;
                                    for (uint32_t k = 0; k < pos_map.via.map.size; ++k)
                                    {
                                        auto &coord_kv = pos_map.via.map.ptr[k];
                                        if (coord_kv.key.type == msgpack::type::STR)
                                        {
                                            std::string coord_name(coord_kv.key.via.str.ptr, coord_kv.key.via.str.size);
                                            double value = 0.0;
                                            if (coord_kv.val.type == msgpack::type::FLOAT)
                                            {
                                                value = coord_kv.val.via.f64;
                                            }
                                            else if (coord_kv.val.type == msgpack::type::POSITIVE_INTEGER)
                                            {
                                                value = static_cast<double>(coord_kv.val.via.u64);
                                            }
                                            else if (coord_kv.val.type == msgpack::type::NEGATIVE_INTEGER)
                                            {
                                                value = static_cast<double>(coord_kv.val.via.i64);
                                            }

                                            if (coord_name == "x")
                                                position.x = value;
                                            else if (coord_name == "y")
                                                position.y = value;
                                            else if (coord_name == "z")
                                                position.z = value;
                                        }
                                    }
                                }
                            }
                        }

                        // Print out the extracted information
                        std::cout << "Dummy with ID: " << id << " velocity set" << std::endl;
                        std::cout << "New position/velocity: " << position << std::endl;
                    }
                }
                catch (std::exception &e)
                {
                    std::cout << "Error parsing DummyVelocitySet data: " << e.what() << std::endl;
                }
            }
            else
            {
                std::cout << "Unknown response variant: " << key_str << "\n";
                std::cout << "Value: " << kv.val << "\n";
            }
        }
    }

    if (!found_variant)
    {
        std::cout << "No recognized response variant found in the response_data\n";
    }
}

// Helper function to interpret MessagePack response with improved parsing
void interpret_msgpack_response(const char *data, size_t size)
{
    try
    {
        // Create msgpack zone for memory allocation
        msgpack::zone zone;

        // Unpack the data
        msgpack::object obj = msgpack::unpack(data, size, nullptr, &zone).get();

        std::cout << "\n===== MSGPACK INTERPRETATION =====\n";

        // Check if this is a structured response from the server
        if (obj.type == msgpack::type::MAP)
        {
            std::cout << "Server response detected (map with " << obj.via.map.size << " fields)\n";

            bool has_response_info = false;
            bool has_response_data = false;

            // Look for response_info and response_data fields
            for (uint32_t i = 0; i < obj.via.map.size; ++i)
            {
                auto &kv = obj.via.map.ptr[i];
                if (kv.key.type == msgpack::type::STR)
                {
                    std::string key_str(kv.key.via.str.ptr, kv.key.via.str.size);

                    if (key_str == "response_info")
                    {
                        has_response_info = true;
                        std::cout << "\n----- Response Info -----\n";

                        // Process response_info if it's in the expected format
                        if (kv.val.type == msgpack::type::MAP)
                        {
                            for (uint32_t j = 0; j < kv.val.via.map.size; ++j)
                            {
                                auto &info_kv = kv.val.via.map.ptr[j];
                                if (info_kv.key.type == msgpack::type::STR)
                                {
                                    std::string info_key(info_kv.key.via.str.ptr, info_kv.key.via.str.size);
                                    std::cout << "  " << info_key << ": " << info_kv.val << std::endl;
                                }
                            }
                        }
                        else
                        {
                            std::cout << "  (Unexpected format)" << std::endl;
                        }
                    }
                    else if (key_str == "response_data")
                    {
                        has_response_data = true;
                        std::cout << "\n----- Response Data -----\n";

                        // Process the response data with our specialized analyzer
                        analyze_response_data(kv.val);
                    }
                    else
                    {
                        // Any other fields we don't specifically handle
                        std::cout << "\n----- " << key_str << " -----\n";
                        std::cout << "  Value: " << kv.val << std::endl;
                    }
                }
            }

            if (!has_response_info && !has_response_data)
            {
                std::cout << "Response does not match the expected server message format\n";
                std::cout << "Raw structure: " << obj << std::endl;
            }
        }
        else if (obj.type == msgpack::type::ARRAY)
        {
            std::cout << "Response is an array with " << obj.via.array.size << " elements\n";
            for (uint32_t i = 0; i < obj.via.array.size; ++i)
            {
                std::cout << "Element " << i << ": " << obj.via.array.ptr[i] << std::endl;
            }
        }
        else
        {
            std::cout << "Response is a single "
                      << (obj.type == msgpack::type::NIL ? "NIL" : obj.type == msgpack::type::BOOLEAN                                                       ? "BOOLEAN"
                                                               : obj.type == msgpack::type::POSITIVE_INTEGER || obj.type == msgpack::type::NEGATIVE_INTEGER ? "INTEGER"
                                                               : obj.type == msgpack::type::FLOAT32 || obj.type == msgpack::type::FLOAT64                   ? "FLOAT"
                                                               : obj.type == msgpack::type::STR                                                             ? "STRING"
                                                               : obj.type == msgpack::type::BIN                                                             ? "BINARY"
                                                               : obj.type == msgpack::type::EXT                                                             ? "EXTENSION"
                                                                                                                                                            : "UNKNOWN")
                      << " value\n";
            std::cout << "Value: " << obj << std::endl;
        }

        std::cout << "================================\n";
    }
    catch (std::exception &e)
    {
        std::cout << "Failed to interpret MessagePack data: " << e.what() << std::endl;
    }
}

// Helper to log WebSocket state transitions for debugging
class WebSocketLogger
{
public:
    static void log_state(const std::string &message)
    {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;
        auto timer = std::time(nullptr);
        auto local_time = *std::localtime(&timer);

        std::cout << "["
                  << std::setw(2) << std::setfill('0') << local_time.tm_hour << ":"
                  << std::setw(2) << std::setfill('0') << local_time.tm_min << ":"
                  << std::setw(2) << std::setfill('0') << local_time.tm_sec << "."
                  << std::setw(3) << std::setfill('0') << ms
                  << "] " << message << std::endl;
    }
};

int main()
{
    try
    {
        std::cout << "Advanced WebSocket Client - MessagePack Test" << std::endl;
        std::cout << "-------------------------------------------" << std::endl;

        // Initialize Boost.Asio
        boost::asio::io_context ioc;
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};

        // Connect to server
        WebSocketLogger::log_state("Connecting to localhost:8082/msgpack...");
        auto results = boost::asio::ip::tcp::resolver{ioc}.resolve("localhost", "8082");
        auto ep = boost::asio::connect(ws.next_layer(), results);

        // Set WebSocket options
        ws.set_option(boost::beast::websocket::stream_base::timeout::suggested(
            boost::beast::role_type::client));

        // Perform the handshake
        ws.handshake("localhost:" + std::to_string(ep.port()), "/msgpack");
        WebSocketLogger::log_state("Connected successfully!");

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
        std::string requestType;

        switch (choice)
        {
        case 1:
        {
            std::string dummy_id;
            std::cout << "Enter dummy ID: ";
            std::cin.ignore(); // Clear input buffer
            std::getline(std::cin, dummy_id);

            requestType = "SpawnDummy";
            WebSocketLogger::log_state("Creating SpawnDummy message with ID: " + dummy_id);
            serialize_spawn_dummy(buffer, dummy_id);
            break;
        }

        case 2:
        {
            double fps;
            std::cout << "Enter FPS value: ";
            std::cin >> fps;

            requestType = "SetClientFPS";
            WebSocketLogger::log_state("Creating SetClientFPS message with value: " + std::to_string(fps));
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

            requestType = "DummySetVelocity";
            WebSocketLogger::log_state("Creating DummySetVelocity message with ID: " + dummy_id);
            serialize_dummy_set_velocity(buffer, dummy_id, x, y, z);
            break;
        }

        default:
            requestType = "SpawnDummy (default)";
            WebSocketLogger::log_state("Invalid choice, defaulting to SpawnDummy");
            serialize_spawn_dummy(buffer, "default_dummy");
            break;
        }

        // Print debug info about the binary data we're sending
        std::cout << "\nSending MessagePack binary data (" + requestType + "):" << std::endl;
        std::cout << "Size: " << buffer.size() << " bytes" << std::endl;
        print_hex_dump(buffer.data(), buffer.size());

        // Send the binary message
        WebSocketLogger::log_state("Sending message to server...");
        ws.write(boost::asio::buffer(buffer.data(), buffer.size()));
        WebSocketLogger::log_state("Message sent successfully");

        WebSocketLogger::log_state("Waiting for server response (5 second timeout)...");

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

            WebSocketLogger::log_state("Response received!");
            std::cout << "\n===== RESPONSE RECEIVED =====\n";
            std::cout << "Size: " << response_buffer.data().size() << " bytes\n";

            // Print detailed hex dump of the response
            print_hex_dump(
                static_cast<const char *>(response_buffer.data().data()),
                response_buffer.data().size());

            // Attempt to interpret the MessagePack data with our improved method
            interpret_msgpack_response(
                static_cast<const char *>(response_buffer.data().data()),
                response_buffer.data().size());
        }
        catch (boost::system::system_error &e)
        {
            WebSocketLogger::log_state("Error reading response: " + std::string(e.what()));
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
            WebSocketLogger::log_state("General error: " + std::string(e.what()));
        }

        if (!response_received)
        {
            std::cout << "No response was received from the server." << std::endl;
        }

        // Close the WebSocket connection
        WebSocketLogger::log_state("Closing connection...");
        try
        {
            ws.close(boost::beast::websocket::close_code::normal);
            WebSocketLogger::log_state("Connection closed gracefully");
        }
        catch (std::exception &e)
        {
            WebSocketLogger::log_state("Error while closing connection: " + std::string(e.what()));
        }
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
