#include "http-adapter.h"
#include <iostream>

bool createUser(const std::string& username, const std::string& password)
{
    std::string body = R"({"username":")" + username + R"(","password":")" + password + R"("})";

    cpr::Response response = cpr::Post(
        cpr::Url{"http://localhost:8080/user"},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Body{body});

    std::cout << "createUser response: " << response.text << " (Status: " << response.status_code << ")" << std::endl;

    return response.status_code == 200;
}

std::optional<std::string> authenticateOnServer(const std::string& username, const std::string& password)
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
            std::cout << "authenticateOnServer response: " << response.text << " (Status: " << response.status_code << ")" << std::endl;
            return authHeader;
        }
    }

    std::cout << "authenticateOnServer response: " << response.text << " (Status: " << response.status_code << ")" << std::endl;

    return std::nullopt;
}

std::string createLobby(const std::string& lobbyName, const std::optional<std::string>& lobbyPassword, const std::string& jwtToken)
{
    std::string body = R"({"lobbyName":")" + lobbyName + R"(")";
    if (lobbyPassword)
    {
        body += R"(,"lobbyPassword":")" + *lobbyPassword + R"(")";
    }
    body += "}";
	COMM_LOG("%s",body.c_str());

    cpr::Response response = cpr::Post(
        cpr::Url{"http://localhost:8080/lobbys"},
        cpr::Header{{"Authorization",jwtToken}, {"Content-Type", "application/json"}},
        cpr::Body{body});

    std::cout << "createLobby response: " << response.text << " (Status: " << response.status_code << ")" << std::endl;

    return response.text;
}

/*
int main()
{
    std::string username = "s;dfg123dfgdfg;";
    std::string password = "123";

    createUser(username, password);

    auto token = authenticateOnServer(username, password);

    if (token)
    {
        createLobby("123", std::nullopt, *token);
    }

    return 0;
}
*/
