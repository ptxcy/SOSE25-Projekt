#ifndef HTTP_ADAPTER_H
#define HTTP_ADAPTER_H


#include <cpr/cpr.h>
#include <string>
#include <optional>
#include "base64.h"
#include "base.h"


class Adapter
{
public:
	Adapter(string host,string port);
	bool createUser(const std::string& username, const std::string& password);
	std::optional<std::string> authenticateOnServer(const std::string& username, const std::string& password);
	std::string createLobby(const std::string& lobbyName, const std::optional<std::string>& lobbyPassword, const std::string& jwtToken);
	string joinLobby(const std::string& lobbyName, const std::optional<std::string>& lobbyPassword, const std::string& jwtToken);

private:
	string m_Addr;
};


#endif // HTTP_ADAPTER_H
