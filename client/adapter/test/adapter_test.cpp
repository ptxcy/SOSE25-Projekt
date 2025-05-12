#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <functional>

// Include adapter headers from parent directory
#include "../http-adapter.h"
#include "../websocket-adapter.h"

// For MessagePack data handling (optional)
// #include <msgpack.hpp>

// Test the HTTP adapter
void testHttpAdapter() {
    std::cout << "\n=== Testing HTTP Adapter ===\n" << std::endl;

    // Step 1: Create a user
    std::string username = "testuser";
    std::string password = "testpassword";
    
    std::cout << "Creating user: " << username << std::endl;
    bool userCreated = createUser(username, password);
    std::cout << "User creation " << (userCreated ? "successful" : "failed") << std::endl;
    
    // Step 2: Authenticate the user
    std::cout << "\nAuthenticating user: " << username << std::endl;
    auto token = authenticateOnServer(username, password);
    if (token) {
        std::cout << "Authentication successful" << std::endl;
        std::cout << "Token: " << token->substr(0, 10) << "..." << std::endl;
    } else {
        std::cout << "Authentication failed" << std::endl;
        return;
    }
    
    // Step 3: Create a lobby
    std::string lobbyName = "testlobby";
    std::optional<std::string> lobbyPassword = std::nullopt;
    
    std::cout << "\nCreating lobby: " << lobbyName << std::endl;
    bool lobbyCreated = createLobby(lobbyName, lobbyPassword, *token);
    std::cout << "Lobby creation " << (lobbyCreated ? "successful" : "failed") << std::endl;
    
    // Step 4: Test the convenience function
    std::cout << "\nTesting the complete auth and lobby process..." << std::endl;
    auto completeToken = completeAuthAndLobbyProcess(username, password, false);
    if (completeToken) {
        std::cout << "Complete process successful" << std::endl;
    } else {
        std::cout << "Complete process failed" << std::endl;
    }
}

// Test the WebSocket adapter
void testWebSocketAdapter() {
    std::cout << "\n=== Testing WebSocket Adapter ===\n" << std::endl;
    
    // First, get an authentication token
    std::string username = "testuser";
    std::string password = "testpassword";
    
    std::cout << "Getting authentication token..." << std::endl;
    auto token = authenticateOnServer(username, password);
    if (!token) {
        std::cout << "Authentication failed, cannot test WebSocket" << std::endl;
        return;
    }
    std::cout << "Authentication successful" << std::endl;
    
    // Set up message callback
    auto messageCallback = [](const std::string& message, bool isBinary) {
        std::cout << "Received " << (isBinary ? "binary" : "text") << " message: ";
        if (isBinary) {
            std::cout << "[binary data, " << message.size() << " bytes]" << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    };
    
    // Set up connection callback
    auto connectionCallback = [](bool success) {
        std::cout << "Connection " << (success ? "established" : "failed") << std::endl;
    };

    // Test connection to /calculate endpoint
    std::cout << "\nConnecting to /calculate endpoint..." << std::endl;
    auto calculateClient = connectToCalculateWebSocket(*token, "localhost", "8080", messageCallback, connectionCallback);
    
    // Wait a moment for the connection to establish
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    if (calculateClient && calculateClient->is_connected()) {
        std::cout << "Sending test message to /calculate..." << std::endl;
        calculateClient->send_message("Hello from test client", false);
        
        // Wait to receive messages
        std::cout << "Waiting for responses (5 seconds)..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        // Close the connection
        std::cout << "Closing connection to /calculate..." << std::endl;
        calculateClient->close();
    } else {
        std::cout << "Failed to connect to /calculate endpoint" << std::endl;
    }
    
    // Test connection to calculation unit
    std::cout << "\nConnecting to calculation unit..." << std::endl;
    auto calcUnitClient = connectToCalculationUnit("localhost:8082", *token, messageCallback, connectionCallback);
    
    // Wait a moment for the connection to establish
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    if (calcUnitClient && calcUnitClient->is_connected()) {
        std::cout << "Sending test binary message to calculation unit..." << std::endl;
        // Create a simple binary message (normally this would be MessagePack encoded data)
        std::vector<uint8_t> binaryData = {0x01, 0x02, 0x03, 0x04, 0x05};
        sendMessagePackToCalculationUnit(calcUnitClient, binaryData);
        
        // Wait to receive messages
        std::cout << "Waiting for responses (5 seconds)..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        // Close the connection
        std::cout << "Closing connection to calculation unit..." << std::endl;
        closeCalculationUnitConnection(calcUnitClient);
    } else {
        std::cout << "Failed to connect to calculation unit" << std::endl;
    }
}

int main() {
    // Ensure the backend services are running before testing
    std::cout << "Make sure the backend services are running (./start-backend.sh)" << std::endl;
    std::cout << "Press Enter to continue...";
    std::cin.get();
    
    // Test HTTP adapter
    testHttpAdapter();
    
    // Test WebSocket adapter
    testWebSocketAdapter();
    
    return 0;
}
