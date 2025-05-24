#include "http-adapter.h"
#include <iostream>

bool createUser(const std::string &username, const std::string &password)
{
    std::string body = R"({"username":")" + username + R"(","password":")" + password + R"("})";

    cpr::Response response = cpr::Post(
        cpr::Url{"http://localhost:8080/user"},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Body{body});

    return response.status_code == 200;
}

std::optional<std::string> authenticateOnServer(const std::string &username, const std::string &password)
{
    std::string basicAuth = "Basic " + base64::encode(username + ":" + password);

    cpr::Response response = cpr::Get(
        cpr::Url{"http://localhost:8080/authenticate"},
        cpr::Header{{"Authorization", basicAuth}});

    if (response.status_code == 200)
    {
        auto authHeader = response.header["Authorization"];
        if (authHeader.find("Bearer ") == 0)
        {
            return authHeader.substr(7);
        }
    }

    return std::nullopt;
}

bool createLobby(const std::string &lobbyName, const std::optional<std::string> &lobbyPassword, const std::string &jwtToken)
{
    std::string body = R"({"lobbyname":")" + lobbyName + R"(")";
    if (lobbyPassword)
    {
        body += R"(,"lobbypassword":")" + *lobbyPassword + R"(")";
    }
    body += "}";

    cpr::Response response = cpr::Post(
        cpr::Url{"http://localhost:8080/lobbys"},
        cpr::Header{{"Authorization", "Bearer " + jwtToken}, {"Content-Type", "application/json"}},
        cpr::Body{body});

    return response.status_code == 200;
}

int main()
{
    std::string username = "Puffito";
    std::string password = "123";

    createUser(username, password);

    auto token = authenticateOnServer(username, password);

    if (token)
    {
        bool lobbyCreated = createLobby("TestLobby", std::nullopt, *token);
        std::cout << "Lobby created: " << (lobbyCreated ? "Yes" : "No") << std::endl;
    }

    return 0;
}
