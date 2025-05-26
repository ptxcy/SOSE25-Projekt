#include "http-adapter.h"


Adapter::Adapter(string host,string port)
	: m_Addr("http://"+host+':'+port)
{  }

bool Adapter::createUser(const std::string& username, const std::string& password)
{
    std::string body = R"({"username":")" + username + R"(","password":")" + password + R"("})";

    cpr::Response response = cpr::Post(
        cpr::Url{m_Addr+"/user"},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Body{body});

    std::cout << "createUser response: " << response.text << " (Status: " << response.status_code << ")" << std::endl;

    return response.status_code == 200;
}

std::optional<std::string> Adapter::authenticateOnServer(const std::string& username, const std::string& password)
{
    std::string basicAuth = "Basic " + base64::encode(username + ":" + password);

    cpr::Response response = cpr::Get(
        cpr::Url{m_Addr+"/authenticate"},
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

std::string Adapter::createLobby(const std::string& lobbyName, const std::optional<std::string>& lobbyPassword, const std::string& jwtToken)
{
    std::string body = R"({"lobbyName":")" + lobbyName + R"(")";
    if (lobbyPassword)
    {
        body += R"(,"lobbyPassword":")" + *lobbyPassword + R"(")";
    }
    body += "}";
	COMM_LOG("%s",body.c_str());

    cpr::Response response = cpr::Post(
        cpr::Url{m_Addr+"/lobbys"},
        cpr::Header{{"Authorization",jwtToken}, {"Content-Type", "application/json"}},
        cpr::Body{body});

    std::cout << "createLobby response: " << response.text << " (Status: " << response.status_code << ")" << std::endl;

    return response.text;
}

std::string Adapter::joinLobby(const std::string& lobbyName, const std::optional<std::string>& lobbyPassword, const std::string& jwtToken)
{
    std::string body = R"({"lobbyName":")" + lobbyName + R"(")";
    if (lobbyPassword)
    {
        body += R"(,"lobbyPassword":")" + *lobbyPassword + R"(")";
    }
    body += "}";
	COMM_LOG("%s",body.c_str());

    cpr::Response response = cpr::Put(
        cpr::Url{m_Addr+"/lobbys"},
        cpr::Header{{"Authorization",jwtToken}, {"Content-Type", "application/json"}},
        cpr::Body{body});

    std::cout << "createLobby response: " << response.text << " (Status: " << response.status_code << ")" << std::endl;

    return response.text;
}
