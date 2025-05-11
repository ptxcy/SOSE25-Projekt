#include "mock_http_adapter.h"
#include <iostream>

// Mock implementation of user creation
bool createUser(const std::string& username, const std::string& password) {
    std::cout << "MOCK: Creating user with username: " << username << std::endl;
    return true; // Always succeed in mock version
}

// Mock implementation of authentication
std::optional<std::string> authenticateOnServer(const std::string& username, const std::string& password) {
    std::cout << "MOCK: Authenticating user: " << username << std::endl;
    // Return a fake JWT token
    return "mock.jwt.token";
}

// Mock implementation of lobby creation
bool createLobby(const std::string& lobbyName, const std::optional<std::string>& lobbyPassword, const std::string& jwtToken) {
    std::cout << "MOCK: Creating lobby: " << lobbyName;
    if (lobbyPassword) {
        std::cout << " with password";
    }
    std::cout << " using token: " << jwtToken << std::endl;
    return true; // Always succeed in mock version
}

// Mock implementation of the convenience function
std::optional<std::string> completeAuthAndLobbyProcess(
    const std::string& username, 
    const std::string& password,
    bool createNewUser,
    const std::optional<std::string>& lobbyName,
    const std::optional<std::string>& lobbyPassword
) {
    std::cout << "MOCK: Processing authentication workflow for user: " << username << std::endl;
    
    if (createNewUser) {
        createUser(username, password);
    }
    
    auto token = authenticateOnServer(username, password);
    
    if (lobbyName) {
        createLobby(*lobbyName, lobbyPassword, *token);
    }
    
    return token;
}
