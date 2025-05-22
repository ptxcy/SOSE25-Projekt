#include "http-adapter.h"
#include <iostream>

// Funktion zur Benutzererstellung
bool createUser(const std::string& username, const std::string& password) {
    std::string body = R"({"username":")" + username + R"(","password":")" + password + R"("})";

    cpr::Response response = cpr::Post(
        cpr::Url{"http://localhost:8080/user"},
        cpr::Header{{"Content-Type", "application/json"}},
        cpr::Body{body}
    );

    return response.status_code == 200; // Erfolgreiche Erstellung
}

// Funktion zur Authentifizierung
std::optional<std::string> authenticateOnServer(const std::string& username, const std::string& password) {
    std::string basicAuth = "Basic " + cppcodec::base64_rfc4648::encode(username + ":" + password);
    
    cpr::Response response = cpr::Get(
        cpr::Url{"http://localhost:8080/authenticate"},
        cpr::Header{{"Authorization", basicAuth}}
    );

    if (response.status_code == 200) {
        auto authHeader = response.header["Authorization"];
        if (authHeader.find("Bearer ") == 0) {
            return authHeader.substr(7); // Extrahiert den Token nach "Bearer "
        }
    }

    return std::nullopt; // Authentifizierung fehlgeschlagen
}

// Funktion zur Lobby-Erstellung mit JWT-Token
bool createLobby(const std::string& lobbyName, const std::optional<std::string>& lobbyPassword, const std::string& jwtToken) {
    std::string body = R"({"lobbyname":")" + lobbyName + R"(")";
    if (lobbyPassword) {
        body += R"(,"lobbypassword":")" + *lobbyPassword + R"(")";
    }
    body += "}";

    cpr::Response response = cpr::Post(
        cpr::Url{"http://localhost:8080/lobbys"},
        cpr::Header{{"Authorization", "Bearer " + jwtToken}, {"Content-Type", "application/json"}},
        cpr::Body{body}
    );

    return response.status_code == 200; // Erfolgreiche Lobby-Erstellung
}

/**
 * Funktion für den gesamten Authentifizierungs- und Lobby-Erstellungsprozess
 * 
 * @param username Nutzername
 * @param password Passwort
 * @param createNewUser Wenn true, wird versucht einen neuen Nutzer zu erstellen
 * @param lobbyName Name der zu erstellenden Lobby (optional)
 * @param lobbyPassword Passwort für die Lobby (optional)
 * @return JWT Token falls erfolgreich, std::nullopt bei Fehlern
 */
std::optional<std::string> completeAuthAndLobbyProcess(
    const std::string& username, 
    const std::string& password,
    bool createNewUser = false,
    const std::optional<std::string>& lobbyName = std::nullopt,
    const std::optional<std::string>& lobbyPassword = std::nullopt
) {
    // Schritt 1: Nutzer erstellen, falls gewünscht
    if (createNewUser) {
        bool userCreated = createUser(username, password);
        if (!userCreated) {
            std::cerr << "Benutzererstellung fehlgeschlagen" << std::endl;
            // Weiter versuchen, falls der Benutzer bereits existiert
        }
    }
    
    // Schritt 2: Authentifizieren und Token erhalten
    auto token = authenticateOnServer(username, password);
    if (!token) {
        std::cerr << "Authentifizierung fehlgeschlagen" << std::endl;
        return std::nullopt;
    }
    
    // Schritt 3: Lobby erstellen, falls gewünscht
    if (lobbyName) {
        bool lobbyCreated = createLobby(*lobbyName, lobbyPassword, *token);
        if (!lobbyCreated) {
            std::cerr << "Lobby-Erstellung fehlgeschlagen" << std::endl;
            // Trotzdem Token zurückgeben, da Authentifizierung erfolgreich war
        }
    }
    
    return token;
}