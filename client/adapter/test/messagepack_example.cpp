#include <iostream>
#include <vector>
#include "../message-pack-helper.h"
#include "../websocket-adapter.h"

// Einfache Struktur für Test-Daten
struct CalculationRequest {
    int operation;
    std::vector<double> values;
    std::string requestId;
    
    MSGPACK_DEFINE(operation, values, requestId);  // MessagePack Serialisierungsdefinition
};

// Callback-Funktion für empfangene Nachrichten
void handleMessage(const std::string& message, bool is_binary) {
    if (is_binary) {
        std::cout << "Binary message received (MessagePack data)" << std::endl;
        try {
            // Versuche, die Nachricht als MessagePack zu deserialisieren
            auto response = msgpack_helper::unpack_from_message<std::map<std::string, msgpack::object>>(
                message, is_binary
            );
            
            // Zeige den Typ der Antwort an (z.B. "result" oder "error")
            for (const auto& pair : response) {
                std::cout << "Key: " << pair.first << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error unpacking message: " << e.what() << std::endl;
        }
    }
    else {
        std::cout << "Text message received: " << message << std::endl;
    }
}

// Callback für Verbindungsstatus
void handleConnection(bool connected) {
    std::cout << "Connection status: " << (connected ? "Connected" : "Disconnected") << std::endl;
}

int main(int argc, char* argv[]) {
    // Authentifizierungstoken (muss vorher vom Authentifizierungsserver geholt werden)
    std::string token = "your_jwt_token_here";
    
    if (argc > 1) {
        token = argv[1];
    }
    
    // Verbindung zum Calculation Unit herstellen
    auto client = connectToCalculationUnit(
        "localhost:8082",  // BaseUrl des Calculation Units
        token,             // Bearer-Token
        handleMessage,     // Nachrichtenverarbeitung-Callback
        handleConnection   // Verbindungsstatus-Callback
    );
    
    if (!client || !client->is_connected()) {
        std::cerr << "Failed to connect to calculation unit" << std::endl;
        return 1;
    }
    
    // Warte kurz, bis die Verbindung hergestellt ist
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Erstelle eine Beispiel-Berechnungsanfrage
    CalculationRequest request;
    request.operation = 1;        // Beispiel: 1 für Addition
    request.values = {10.5, 20.3, 30.1};
    request.requestId = "req-123";
    
    // Serialisiere die Anfrage in MessagePack-Format
    std::vector<uint8_t> messagePackData = msgpack_helper::pack(request);
    
    std::cout << "Sending calculation request..." << std::endl;
    // Sende die MessagePack-Daten an die Calculation Unit
    bool sent = sendMessagePackToCalculationUnit(client, messagePackData);
    
    if (!sent) {
        std::cerr << "Failed to send message" << std::endl;
        return 1;
    }
    
    std::cout << "Request sent successfully. Waiting for response..." << std::endl;
    
    // Warte auf die Antwort (in einem echten Programm würde man asynchron arbeiten)
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Verbindung schließen
    closeCalculationUnitConnection(client);
    
    return 0;
}
