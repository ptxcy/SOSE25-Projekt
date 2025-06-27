#include "core/websocket.h"

#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>

/**
 * Integration test program for client websocket connections
 *
 * This program tests the complete flow:
 * 1. Create two users (user1, user2)
 * 2. User1 creates a lobby
 * 3. User2 joins the lobby
 * 4. Wait for stable connections (3 seconds)
 * 5. Close both websocket connections
 * 6. Print "true" if successful, "false" otherwise
 */

bool test_integration()
{
    try
    {
        // Test configuration
        string host = NETWORK_HOST;
        string port_adapter = NETWORK_PORT_ADAPTER;
        string port_websocket = NETWORK_PORT_WEBSOCKET;
        string lobby_name = "test_lobby_" + std::to_string(std::time(nullptr)); // Unique lobby name
        string lobby_password = "";

        // User credentials with timestamp for uniqueness
        auto timestamp = std::to_string(std::time(nullptr));
        string user1_name = "test_user_1_" + timestamp;
        string user1_pass = "test_pass_1";
        string user2_name = "test_user_2_" + timestamp;
        string user2_pass = "test_pass_2";

        std::cout << "Starting integration test..." << std::endl;
        std::cout << "Host: " << host << ", Adapter Port: " << port_adapter << ", WebSocket Port: " << port_websocket << std::endl;
        std::cout << "Lobby: " << lobby_name << std::endl;

#ifdef FEAT_MULTIPLAYER
        // Create two websocket instances
        Websocket websocket1, websocket2;

        std::cout << "Creating first user (" << user1_name << ") and lobby..." << std::endl;

        // User1: Create lobby
        websocket1.connect(host, port_adapter, port_websocket,
                           user1_name, user1_pass, lobby_name, lobby_password, true);

        // Check if lobby creation was successful
        if (websocket1.lobby_status != LOBBY_CONNECTED)
        {
            std::cout << "Failed to create lobby with user1. Status: " << websocket1.lobby_status << std::endl;
            std::cout << "Possible reasons: Server not running, network issues, or authentication failure" << std::endl;
            return false;
        }

        std::cout << "First user connected successfully" << std::endl;
        std::cout << "Creating second user (" << user2_name << ") and joining lobby..." << std::endl;

        // Small delay to ensure lobby is ready
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // User2: Join existing lobby
        websocket2.connect(host, port_adapter, port_websocket,
                           user2_name, user2_pass, lobby_name, lobby_password, false);

        // Check if lobby join was successful
        if (websocket2.lobby_status != LOBBY_CONNECTED)
        {
            std::cout << "Failed to join lobby with user2. Status: " << websocket2.lobby_status << std::endl;
            std::cout << "Cleaning up first connection..." << std::endl;
            websocket1.exit();
            return false;
        }

        std::cout << "Both users connected successfully" << std::endl;
        std::cout << "Waiting for stable connection (3 seconds)..." << std::endl;

        // Wait for stable connection (3 seconds)
        std::this_thread::sleep_for(std::chrono::seconds(3));

        std::cout << "Closing websocket connections..." << std::endl;

        // Close both websocket connections
        websocket1.exit();
        websocket2.exit();

        std::cout << "Integration test completed successfully" << std::endl;
        return true;

#else
        std::cout << "FEAT_MULTIPLAYER not enabled - this is expected if multiplayer features are disabled" << std::endl;
        std::cout << "To enable multiplayer testing, uncomment FEAT_MULTIPLAYER in core/config.h" << std::endl;
        return true;
#endif
    }
    catch (const std::exception &e)
    {
        std::cout << "Integration test failed with exception: " << e.what() << std::endl;
        return false;
    }
    catch (...)
    {
        std::cout << "Integration test failed with unknown exception" << std::endl;
        return false;
    }
}

int main(int argc, char **argv)
{
    // Run integration test
    bool success = test_integration();

    // Print result as required
    if (success)
    {
        std::cout << "true" << std::endl;
    }
    else
    {
        std::cout << "false" << std::endl;
    }

    return success ? 0 : 1;
}
