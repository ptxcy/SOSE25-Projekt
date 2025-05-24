#ifndef HTTP_ADAPTER_H
#define HTTP_ADAPTER_H

#include <cpr/cpr.h>
#include "base64.h"
#include <string>
#include <optional>

// User creation function
bool createUser(const std::string& username, const std::string& password);

// Authentication function
std::optional<std::string> authenticateOnServer(const std::string& username, const std::string& password);

// Lobby creation function
bool createLobby(const std::string& lobbyName, const std::optional<std::string>& lobbyPassword, const std::string& jwtToken);

// Convenience function for authentication workflow
std::optional<std::string> completeAuthAndLobbyProcess(
    const std::string& username,
    const std::string& password,
    bool createNewUser = false,
    const std::optional<std::string>& lobbyName = std::nullopt,
    const std::optional<std::string>& lobbyPassword = std::nullopt
);

#endif // HTTP_ADAPTER_H
