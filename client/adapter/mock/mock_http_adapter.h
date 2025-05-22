#ifndef MOCK_HTTP_ADAPTER_H
#define MOCK_HTTP_ADAPTER_H

#include <string>
#include <optional>

// Mock user creation function
bool createUser(const std::string& username, const std::string& password);

// Mock authentication function
std::optional<std::string> authenticateOnServer(const std::string& username, const std::string& password);

// Mock lobby creation function
bool createLobby(const std::string& lobbyName, const std::optional<std::string>& lobbyPassword, const std::string& jwtToken);

// Mock convenience function for authentication workflow
std::optional<std::string> completeAuthAndLobbyProcess(
    const std::string& username, 
    const std::string& password,
    bool createNewUser = false,
    const std::optional<std::string>& lobbyName = std::nullopt,
    const std::optional<std::string>& lobbyPassword = std::nullopt
);

#endif // MOCK_HTTP_ADAPTER_H
