#include <iostream>
#include "mock_http_adapter.h"

/**
 * Ein einfaches Beispiel für die Verwendung des Mock HTTP-Adapters.
 * 
 * Dieses Programm demonstriert die typische Verwendung des HTTP-Adapters
 * für die Benutzerauthentifizierung und Lobby-Erstellung, verwendet aber
 * die Mock-Implementierung, die keine externen Abhängigkeiten benötigt.
 */
int main(int argc, char* argv[]) {
    std::string username = "testuser";
    std::string password = "password123";
    
    // Wenn Befehlszeilenargumente übergeben wurden, verwende sie als Benutzername und Passwort
    if (argc >= 3) {
        username = argv[1];
        password = argv[2];
    }
    
    std::cout << "Beginne Authentifizierungsprozess für Benutzer: " << username << std::endl;
    
    // Versuche zuerst, einen neuen Benutzer zu erstellen
    bool userCreated = createUser(username, password);
    if (userCreated) {
        std::cout << "Benutzer erfolgreich erstellt." << std::endl;
    } else {
        std::cout << "Benutzer konnte nicht erstellt werden (existiert möglicherweise bereits)." << std::endl;
    }
    
    // Authentifiziere den Benutzer
    auto token = authenticateOnServer(username, password);
    if (!token) {
        std::cerr << "Authentifizierung fehlgeschlagen!" << std::endl;
        return 1;
    }
    
    std::cout << "Authentifizierung erfolgreich." << std::endl;
    std::cout << "Token: " << *token << std::endl;
    
    // Erstelle eine Lobby
    std::string lobbyName = "TestLobby";
    std::optional<std::string> lobbyPassword = std::string("lobby123");
    
    bool lobbyCreated = createLobby(lobbyName, lobbyPassword, *token);
    if (lobbyCreated) {
        std::cout << "Lobby '" << lobbyName << "' erfolgreich erstellt." << std::endl;
    } else {
        std::cerr << "Lobby konnte nicht erstellt werden." << std::endl;
    }
    
    std::cout << "\nAlternative Methode mit completeAuthAndLobbyProcess:" << std::endl;
    
    // Verwende die Convenience-Funktion für den gesamten Prozess
    auto workflowToken = completeAuthAndLobbyProcess(
        username,
        password,
        false,  // Benutzer nicht neu erstellen
        std::string("AnotherLobby"),
        std::string("anotherpassword")
    );
    
    if (workflowToken) {
        std::cout << "Gesamter Authentifizierungs- und Lobby-Erstellungsprozess erfolgreich." << std::endl;
    } else {
        std::cerr << "Fehler im Authentifizierungs- und Lobby-Erstellungsprozess." << std::endl;
    }
    
    return 0;
}
