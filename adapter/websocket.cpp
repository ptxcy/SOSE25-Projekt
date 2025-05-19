#include "websocket.h"

// Global variable to store the latest server message
ServerMessage g_latestMessage;
std::mutex g_messageMutex;
bool g_hasMessage = false;

namespace WebSocket
{
    // Get information about the latest server message
    bool getMessage(std::string &targetUserId, int &dummyCount)
    {
        std::lock_guard<std::mutex> lock(g_messageMutex);
        if (!g_hasMessage)
        {
            return false;
        }

        targetUserId = g_latestMessage.request_data.target_user_id;
        dummyCount = static_cast<int>(g_latestMessage.request_data.game_objects.dummies.size());
        return true;
    }

    // Get a list of all dummy IDs in the latest message
    std::vector<std::string> getDummyIds()
    {
        std::lock_guard<std::mutex> lock(g_messageMutex);
        std::vector<std::string> ids;

        if (g_hasMessage)
        {
            for (const auto &pair : g_latestMessage.request_data.game_objects.dummies)
            {
                ids.push_back(pair.first);
            }
        }

        return ids;
    }

    // Get position and velocity of a specific dummy
    bool getDummyData(const std::string &dummyId,
                      double &posX, double &posY, double &posZ,
                      double &velX, double &velY, double &velZ)
    {
        std::lock_guard<std::mutex> lock(g_messageMutex);

        if (!g_hasMessage)
        {
            return false;
        }

        auto &dummies = g_latestMessage.request_data.game_objects.dummies;
        auto it = dummies.find(dummyId);
        if (it == dummies.end())
        {
            return false;
        }

        const auto &dummy = it->second;
        posX = dummy.position.x;
        posY = dummy.position.y;
        posZ = dummy.position.z;

        velX = dummy.velocity.x;
        velY = dummy.velocity.y;
        velZ = dummy.velocity.z;

        return true;
    }

    // Print details of the latest server message to stdout
    void printLatestMessage()
    {
        std::lock_guard<std::mutex> lock(g_messageMutex);
        if (g_hasMessage)
        {
            std::cout << "===== Server Message Details =====" << std::endl;

            const auto &msg = g_latestMessage;

            std::cout << "Request Info:" << std::endl;
            std::cout << "  Client sent time: " << msg.request_info.client.sent_time << std::endl;
            std::cout << "  Authproxy sent time: " << msg.request_info.authproxy.sent_time << std::endl;
            std::cout << "  Request sync sent time: " << msg.request_info.request_sync.sent_time << std::endl;
            std::cout << "  Calculation unit sent time: " << msg.request_info.calculation_unit.sent_time << std::endl;

            std::cout << "Request Data:" << std::endl;
            std::cout << "  Target User ID: " << msg.request_data.target_user_id << std::endl;

            std::cout << "  Game Objects:" << std::endl;
            std::cout << "    Dummies:" << std::endl;
            for (const auto &pair : msg.request_data.game_objects.dummies)
            {
                const auto &dummy_id = pair.first;
                const auto &dummy = pair.second;

                std::cout << "      Dummy ID: " << dummy_id << std::endl;
                std::cout << "        Position: (" << dummy.position.x << ", "
                          << dummy.position.y << ", " << dummy.position.z << ")" << std::endl;
                std::cout << "        Velocity: (" << dummy.velocity.x << ", "
                          << dummy.velocity.y << ", " << dummy.velocity.z << ")" << std::endl;
            }

            std::cout << "=================================" << std::endl;
        }
        else
        {
            std::cout << "No message available to print." << std::endl;
        }
    }
}

// Forward declarations for serialization functions
void serialize_spawn_dummy(msgpack::sbuffer &buffer, const std::string &dummy_id);
void serialize_set_client_fps(msgpack::sbuffer &buffer, double fps);
void serialize_dummy_set_velocity(msgpack::sbuffer &buffer, const std::string &dummy_id, double x, double y, double z);

// Forward declarations for queue helper functions
void queueSpawnDummy(const std::string &dummy_id);
void queueSetClientFPS(double fps);
void queueDummySetVelocity(const std::string &dummy_id, double x, double y, double z);

// Helper function to print hex dump
void print_hex_dump(const char *data, size_t size)
{
    std::cout << "Hex dump:" << std::endl;
    for (size_t i = 0; i < size; ++i)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << (static_cast<int>(data[i]) & 0xFF) << " ";
        if ((i + 1) % 16 == 0)
        {
            std::cout << std::endl;
        }
    }
    std::cout << std::dec << std::endl;
}

// Helper function to pack RequestInfo with default values
void pack_default_request_info(msgpack::packer<msgpack::sbuffer> &packer)
{
    packer.pack_map(4);

    packer.pack("client");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);

    packer.pack("authproxy");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);

    packer.pack("request_sync");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);

    packer.pack("calculation_unit");
    packer.pack_map(1);
    packer.pack("sent_time");
    packer.pack(0.0);
}

// Helper function to serialize SpawnDummy message
void serialize_spawn_dummy(msgpack::sbuffer &buffer, const std::string &dummy_id)
{
    msgpack::packer<msgpack::sbuffer> packer(buffer);

    packer.pack_map(2);

    packer.pack("request_info");
    pack_default_request_info(packer);

    packer.pack("request_data");
    packer.pack_map(1);
    packer.pack("SpawnDummy");
    packer.pack_map(1);
    packer.pack("id");
    packer.pack(dummy_id);
}

// Helper function to serialize SetClientFPS message
void serialize_set_client_fps(msgpack::sbuffer &buffer, double fps)
{
    msgpack::packer<msgpack::sbuffer> packer(buffer);

    packer.pack_map(2);

    packer.pack("request_info");
    pack_default_request_info(packer);

    packer.pack("request_data");
    packer.pack_map(1);
    packer.pack("SetClientFPS");
    packer.pack(fps);
}

// Helper function to serialize DummySetVelocity message
void serialize_dummy_set_velocity(msgpack::sbuffer &buffer, const std::string &dummy_id,
                                  double x, double y, double z)
{
    msgpack::packer<msgpack::sbuffer> packer(buffer);

    packer.pack_map(2);

    packer.pack("request_info");
    pack_default_request_info(packer);

    packer.pack("request_data");
    packer.pack_map(1);
    packer.pack("DummySetVelocity");
    packer.pack_map(2);
    packer.pack("id");
    packer.pack(dummy_id);
    packer.pack("position");
    packer.pack_map(3);
    packer.pack("x");
    packer.pack(x);
    packer.pack("y");
    packer.pack(y);
    packer.pack("z");
    packer.pack(z);
}

// Helper functions to queue different types of messages
void queueSpawnDummy(const std::string &dummy_id)
{
    msgpack::sbuffer buffer;
    serialize_spawn_dummy(buffer, dummy_id);
    sendQueue.push(OutgoingMessage::Type::SpawnDummy, std::move(buffer));
}

void queueSetClientFPS(double fps)
{
    msgpack::sbuffer buffer;
    serialize_set_client_fps(buffer, fps);
    sendQueue.push(OutgoingMessage::Type::SetClientFPS, std::move(buffer));
}

void queueDummySetVelocity(const std::string &dummy_id, double x, double y, double z)
{
    msgpack::sbuffer buffer;
    serialize_dummy_set_velocity(buffer, dummy_id, x, y, z);
    sendQueue.push(OutgoingMessage::Type::DummySetVelocity, std::move(buffer));
}

// Parse server response and add to queue
bool parse_server_response(const char *data, size_t size)
{
    try
    {
        // Create a msgpack zone for allocation
        msgpack::zone zone;

        // Unpack the MessagePack data
        msgpack::object obj = msgpack::unpack(data, size, nullptr, &zone).get();

        // Debug output
        std::cout << "Received MessagePack object: " << obj << std::endl;

        // Try to convert to our ServerMessage structure
        ServerMessage message;
        obj.convert(message);

        // Add the parsed message to our queue
        responseQueue.push(message);

        // Store the latest message for the WebSocket namespace functions
        {
            std::lock_guard<std::mutex> lock(g_messageMutex);
            g_latestMessage = message;
            g_hasMessage = true;
        }

        std::cout << "Successfully parsed server response and added to queue." << std::endl;
        std::cout << "Queue size: " << responseQueue.size() << std::endl;

        // Print some details about the parsed message
        std::cout << "Target User ID: " << message.request_data.target_user_id << std::endl;
        std::cout << "Number of dummies: " << message.request_data.game_objects.dummies.size() << std::endl;

        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error parsing server response: " << e.what() << std::endl;
        return false;
    }
}

// Print information about a server message
void print_server_message(const ServerMessage &message)
{
    std::cout << "===== Server Message Details =====" << std::endl;

    std::cout << "Request Info:" << std::endl;
    std::cout << "  Client sent time: " << message.request_info.client.sent_time << std::endl;
    std::cout << "  Authproxy sent time: " << message.request_info.authproxy.sent_time << std::endl;
    std::cout << "  Request sync sent time: " << message.request_info.request_sync.sent_time << std::endl;
    std::cout << "  Calculation unit sent time: " << message.request_info.calculation_unit.sent_time << std::endl;

    std::cout << "Request Data:" << std::endl;
    std::cout << "  Target User ID: " << message.request_data.target_user_id << std::endl;

    std::cout << "  Game Objects:" << std::endl;
    std::cout << "    Dummies:" << std::endl;
    for (const auto &dummy_pair : message.request_data.game_objects.dummies)
    {
        const auto &dummy_id = dummy_pair.first;
        const auto &dummy = dummy_pair.second;

        std::cout << "      Dummy ID: " << dummy_id << std::endl;
        std::cout << "        Position: (" << dummy.position.x << ", "
                  << dummy.position.y << ", " << dummy.position.z << ")" << std::endl;
        std::cout << "        Velocity: (" << dummy.velocity.x << ", "
                  << dummy.velocity.y << ", " << dummy.velocity.z << ")" << std::endl;
    }

    std::cout << "=================================" << std::endl;
}

// Externally accessible version of print_server_message
void printServerMessage(const ServerMessage &message)
{
    print_server_message(message);
}

// Function to start the websocket adapter - can be called from other files
int startWebsocketAdapter()
{
    try
    {
        // Use the global atomic flag to control the WebSocket loop

        // Queue an initial message to test the connection
        std::cout << "Queueing initial test message..." << std::endl;
        queueSpawnDummy("test_dummy_" + std::to_string(std::time(nullptr)));

        // Main WebSocket loop
        std::cout << "Starting WebSocket client loop..." << std::endl;
        while (true)
        {
            try
            {
                // Set up a new connection
                boost::asio::io_context ioc;
                boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};

                std::cout << "Connecting to WebSocket server at localhost:8082/msgpack..." << std::endl;
                auto results = boost::asio::ip::tcp::resolver{ioc}.resolve("localhost", "8082");
                auto ep = boost::asio::connect(ws.next_layer(), results);
                ws.handshake("localhost:" + std::to_string(ep.port()), "/msgpack");
                ws.binary(true); // We're sending binary data (MessagePack)

                std::cout << "Connected to server successfully!" << std::endl;

                // Inner processing loop - continues until connection error or shutdown
                while (true)
                {
                    // Check if there are messages to send
                    OutgoingMessage outMsg;
                    bool has_message = sendQueue.waitAndPop(outMsg, 1000); // Wait up to 1 second for a message

                    if (has_message)
                    {
                        // Send the message
                        std::cout << "Sending MessagePack data of size: " << outMsg.buffer.size() << " bytes" << std::endl;
                        print_hex_dump(outMsg.buffer.data(), outMsg.buffer.size());

                        ws.write(boost::asio::buffer(outMsg.buffer.data(), outMsg.buffer.size()));
                        std::cout << "Message sent. Waiting for response..." << std::endl;

                        // Create a buffer for the response
                        boost::beast::flat_buffer response_buffer;

                        // Read a response
                        ws.read(response_buffer);

                        // Process the response
                        auto data = response_buffer.data();
                        const char *raw_data = static_cast<const char *>(data.data());
                        size_t data_size = data.size();

                        std::cout << "\n===== RESPONSE RECEIVED =====\n";
                        std::cout << "Size: " << data_size << " bytes\n";
                        print_hex_dump(raw_data, data_size);

                        // Parse the response and add to our receive queue
                        if (parse_server_response(raw_data, data_size))
                        {
                            // Process received message if needed
                            ServerMessage message;
                            if (responseQueue.front(message))
                            {
                                responseQueue.pop(message);
                                print_server_message(message);

                                // Here you could implement logic to react to the received message
                                // For example, if we received a dummy position, we might want to
                                // send a new velocity update in response
                            }
                        }

                        std::cout << "============================\n";
                    }
                    else
                    {
                        // No message to send, so just check if the connection is still alive
                        // by sending a ping
                        try
                        {
                            ws.ping("");
                        }
                        catch (const std::exception &e)
                        {
                            std::cerr << "Connection lost: " << e.what() << std::endl;
                            break; // Break the inner loop to reconnect
                        }
                    }

                    // Sleep for a short time to prevent tight looping
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }

                // Connection has been broken if we get here
                // No need to explicitly close the connection - it's already broken
                // We'll reconnect in the next iteration of the outer loop
                std::cout << "Connection broken. Will attempt to reconnect..." << std::endl;
            }
            catch (std::exception const &e)
            {
                std::cerr << "WebSocket error: " << e.what() << std::endl;
                std::cout << "Reconnecting in 5 seconds..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }
    }
    catch (std::exception const &e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

// Main function to run the WebSocket client
int main()
{
    std::cout << "Starting WebSocket client that sends MessagePack data to localhost:8082/msgpack..." << std::endl;

    // Start a background thread for the WebSocket adapter
    std::thread adapter_thread([]()
                               { startWebsocketAdapter(); });

    // Main thread for user interaction
    std::cout << "WebSocket client running. Enter commands:" << std::endl;
    std::cout << "1. spawn <id> - Spawn a new dummy" << std::endl;
    std::cout << "2. fps <value> - Set client FPS" << std::endl;
    std::cout << "3. velocity <id> <x> <y> <z> - Set dummy velocity" << std::endl;
    std::cout << "4. quit - Exit the program" << std::endl;

    std::string line;
    bool running = true;

    while (running && std::getline(std::cin, line))
    {
        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "spawn")
        {
            std::string id;
            if (iss >> id)
            {
                std::cout << "Queueing SpawnDummy message with ID: " << id << std::endl;
                queueSpawnDummy(id);
            }
            else
            {
                std::cout << "Usage: spawn <id>" << std::endl;
            }
        }
        else if (command == "fps")
        {
            double fps;
            if (iss >> fps)
            {
                std::cout << "Queueing SetClientFPS message with value: " << fps << std::endl;
                queueSetClientFPS(fps);
            }
            else
            {
                std::cout << "Usage: fps <value>" << std::endl;
            }
        }
        else if (command == "velocity")
        {
            std::string id;
            double x, y, z;
            if (iss >> id >> x >> y >> z)
            {
                std::cout << "Queueing DummySetVelocity with ID: " << id << " and position: ("
                          << x << ", " << y << ", " << z << ")" << std::endl;
                queueDummySetVelocity(id, x, y, z);
            }
            else
            {
                std::cout << "Usage: velocity <id> <x> <y> <z>" << std::endl;
            }
        }
        else if (command == "quit" || command == "exit")
        {
            running = false;
            std::cout << "Exiting..." << std::endl;
        }
        else
        {
            std::cout << "Unknown command. Available commands:" << std::endl;
            std::cout << "1. spawn <id> - Spawn a new dummy" << std::endl;
            std::cout << "2. fps <value> - Set client FPS" << std::endl;
            std::cout << "3. velocity <id> <x> <y> <z> - Set dummy velocity" << std::endl;
            std::cout << "4. quit - Exit the program" << std::endl;
        }
    }

    // Wait for the adapter thread to finish
    adapter_thread.join();

    std::cout << "WebSocket client shut down successfully." << std::endl;
    return 0;
}
