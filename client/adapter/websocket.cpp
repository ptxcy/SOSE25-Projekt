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
    clientToServerMessage.push(OutgoingMessage::Type::SpawnDummy, std::move(buffer));
}

void queueSetClientFPS(double fps)
{
    msgpack::sbuffer buffer;
    serialize_set_client_fps(buffer, fps);
    clientToServerMessage.push(OutgoingMessage::Type::SetClientFPS, std::move(buffer));
}

void queueDummySetVelocity(const std::string &dummy_id, double x, double y, double z)
{
    msgpack::sbuffer buffer;
    serialize_dummy_set_velocity(buffer, dummy_id, x, y, z);
    clientToServerMessage.push(OutgoingMessage::Type::DummySetVelocity, std::move(buffer));
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
        serverToClientMessage.push(message);

        std::cout << "Successfully parsed server response and added to queue." << std::endl;
        std::cout << "Queue size: " << serverToClientMessage.size() << std::endl;

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
        std::cout << "Queueing initial test message..." << std::endl;
        queueSpawnDummy("test_dummy_" + std::to_string(std::time(nullptr)));
        std::cout << "Starting WebSocket client loop..." << std::endl;

        boost::asio::io_context ioc;
        boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws{ioc};

        std::cout << "Connecting to WebSocket server at localhost:8082/msgpack..." << std::endl;
        auto results = boost::asio::ip::tcp::resolver{ioc}.resolve("localhost", "8082");
        auto ep = boost::asio::connect(ws.next_layer(), results);
        ws.handshake("localhost:" + std::to_string(ep.port()), "/msgpack");
        ws.binary(true);
        std::cout << "Connected to server successfully!" << std::endl;

        try
        {
            while (true)
            {
                // Check if there are messages to send
                OutgoingMessage outMsg = clientToServerMessage.front();
                if (outMsg)
                {
                    clientToServerMessage.pop();
                    ws.write(boost::asio::buffer(outMsg.buffer.data(), outMsg.buffer.size()));
                }

                // Create a buffer for the response
                boost::beast::flat_buffer response_buffer;
                // Read a response
                ws.read(response_buffer);
                // Process the response
                auto data = response_buffer.data();
                const char *raw_data = static_cast<const char *>(data.data());
                size_t data_size = data.size();
                parse_server_response(raw_data, data_size);
                // Sleep for a short time to prevent tight looping
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        catch (std::exception const &e)
        {
            std::cerr << "WebSocket error: " << e.what() << std::endl;
            std::cout << "Reconnecting in 5 seconds..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
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
   startWebsocketAdapter();
}
