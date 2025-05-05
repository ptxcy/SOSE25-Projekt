#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

// Include both adapters
#include "adapter/http-adapter.cpp"
#include "adapter/websocket-adapter.cpp"

int main() {
    std::cout << "AuthProxy Client Integration Test\n";
    std::cout << "================================\n\n";

    // Configuration
    std::string authProxyUrl = "http://localhost:8080";
    std::string calcUnitUrl = "localhost:8082";  // Will be used for WebSocket
    
    // Test user credentials
    std::string username = "testuser";
    std::string password = "testpassword";
    std::string lobbyName = "TestLobby";
    
    // Step 1: Authenticate with username/password
    std::cout << "1. Authenticating user: " << username << std::endl;
    
    HttpResponse authResponse;
    std::string bearerToken;
    
    try {
        authResponse = authenticateUser(authProxyUrl, username, password);
        
        if (authResponse.http_code == 200) {
            std::cout << "   ✓ Authentication successful!\n";
            std::cout << "   ✓ Response body: " << authResponse.body << "\n";
            
            // Extract bearer token from authentication response
            bearerToken = authResponse.authorizationHeader;
            std::cout << "   ✓ Received bearer token: " << bearerToken << "\n\n";
        } else {
            std::cout << "   ✗ Authentication failed with code: " << authResponse.http_code << std::endl;
            std::cout << "   ✗ Response body: " << authResponse.body << "\n";
            
            if (authResponse.http_code == 401) {
                std::cout << "   ℹ Server is running but credentials are invalid.\n";
                std::cout << "   ℹ This is expected if this is the first run. Creating user instead.\n\n";
                
                // Try to create a user with a dummy token first since server requires Auth
                // This will likely fail but is worth trying
                std::string dummyToken = "dummy_token_for_first_user";
                std::cout << "2. Creating initial user: " << username << std::endl;
                auto createResponse = createUser(authProxyUrl, username, password, dummyToken);
                
                if (createResponse.http_code == 200 || createResponse.http_code == 201) {
                    std::cout << "   ✓ User creation successful!\n";
                    std::cout << "   ✓ Response body: " << createResponse.body << "\n\n";
                    
                    // Now try authentication again
                    std::cout << "3. Authenticating with newly created user\n";
                    authResponse = authenticateUser(authProxyUrl, username, password);
                    
                    if (authResponse.http_code == 200) {
                        bearerToken = authResponse.authorizationHeader;
                        std::cout << "   ✓ Authentication successful after user creation!\n";
                        std::cout << "   ✓ Received bearer token: " << bearerToken << "\n\n";
                    } else {
                        std::cout << "   ✗ Authentication still failed after user creation: " << authResponse.http_code << "\n";
                        return 1;
                    }
                } else {
                    std::cout << "   ✗ User creation failed with code: " << createResponse.http_code << "\n";
                    std::cout << "   ✗ Response: " << createResponse.body << "\n\n";
                    
                    std::cout << "   ℹ You may need to manually create a first user or check if server is running\n";
                    return 1;
                }
            } else {
                std::cout << "   ✗ Unexpected error. Is the AuthProxy server running at " << authProxyUrl << "?\n";
                return 1;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "   ✗ Error during authentication: " << e.what() << std::endl;
        std::cerr << "   ✗ Is the AuthProxy server running at " << authProxyUrl << "?\n";
        return 1;
    }
    
    // Step 2: Create a lobby using the token
    std::cout << "4. Creating lobby: " << lobbyName << std::endl;
    
    try {
        auto lobbyResponse = createLobby(authProxyUrl, lobbyName, bearerToken);
        
        if (lobbyResponse.http_code == 200 || lobbyResponse.http_code == 201) {
            std::cout << "   ✓ Lobby creation successful!\n";
            std::cout << "   ✓ Response: " << lobbyResponse.body << "\n\n";
        } else {
            std::cout << "   ✗ Lobby creation failed with code: " << lobbyResponse.http_code << std::endl;
            std::cout << "   ✗ Response: " << lobbyResponse.body << "\n\n";
            
            // Continue with the test anyway
            std::cout << "   ℹ Continuing with WebSocket test despite lobby creation failure\n\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "   ✗ Error during lobby creation: " << e.what() << std::endl;
        // Continue with the test anyway
    }
    
    // Step 3: Connect to calculation unit using WebSocket
    std::cout << "5. Connecting to calculation unit via WebSocket\n";
    
    // Flag to track if WebSocket connection succeeded
    bool wsConnected = false;
    
    // Sample MessagePack data - this is just a placeholder
    // In a real application, you would use a proper MessagePack library
    std::vector<uint8_t> sampleMessagePack = {
        0x82, // map with 2 elements
        0xa7, 0x63, 0x6f, 0x6d, 0x6d, 0x61, 0x6e, 0x64, // "command"
        0xa4, 0x70, 0x69, 0x6e, 0x67, // "ping"
        0xa4, 0x64, 0x61, 0x74, 0x61, // "data"
        0x91, 0x2a // array with one element: 42
    };
    
    try {
        // Create a WebSocket connection with message and connection status handlers
        auto wsClient = connectToCalculationUnit(
            calcUnitUrl,
            bearerToken,
            // Message handler
            [](const std::string& message, bool isBinary) {
                if (isBinary) {
                    std::cout << "   ✓ Received binary MessagePack data (" << message.size() << " bytes)\n";
                    
                    // Print the first few bytes as hex for debugging
                    std::cout << "   ✓ First bytes: ";
                    for (size_t i = 0; i < std::min(message.size(), size_t(10)); ++i) {
                        printf("%02x ", static_cast<unsigned char>(message[i]));
                    }
                    std::cout << std::endl;
                } else {
                    std::cout << "   ✓ Received text message: " << message << std::endl;
                }
            },
            // Connection handler
            [&wsConnected](bool connected) {
                if (connected) {
                    std::cout << "   ✓ WebSocket connection established successfully!\n";
                    wsConnected = true;
                } else {
                    std::cout << "   ✗ WebSocket connection closed or failed\n";
                    wsConnected = false;
                }
            }
        );
        
        if (wsClient) {
            // Give the connection callback time to be called
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            
            if (wsConnected) {
                // Send a sample MessagePack message
                std::cout << "6. Sending MessagePack data to calculation unit\n";
                if (sendMessagePackToCalculationUnit(wsClient, sampleMessagePack)) {
                    std::cout << "   ✓ MessagePack data sent successfully\n";
                    
                    // Wait a moment to potentially receive a response
                    std::cout << "   ℹ Waiting for potential response...\n";
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                } else {
                    std::cout << "   ✗ Failed to send MessagePack data\n";
                }
                
                // Close the connection
                std::cout << "7. Closing WebSocket connection\n";
                closeCalculationUnitConnection(wsClient);
                std::cout << "   ✓ WebSocket connection closed\n";
            } else {
                std::cout << "   ✗ WebSocket connection was not established\n";
            }
        } else {
            std::cout << "   ✗ Failed to create WebSocket client\n";
            std::cout << "   ℹ Is the calculation unit running at " << calcUnitUrl << "?\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "   ✗ Error during WebSocket communication: " << e.what() << std::endl;
        std::cerr << "   ✗ Is the calculation unit running at " << calcUnitUrl << "?\n";
    }

    std::cout << "\n================================\n";
    std::cout << "Test sequence completed!\n";
    
    return 0;
}