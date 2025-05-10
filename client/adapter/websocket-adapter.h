#ifndef WEBSOCKET_ADAPTER_H
#define WEBSOCKET_ADAPTER_H

#include <cpprest/ws_client.h>
#include <string>
#include <memory>
#include <functional>
#include <vector>

// Define callback types for message and connection events
using MessageCallback = std::function<void(const std::string&, bool)>;
using ConnectionCallback = std::function<void(bool)>; // Bool indicates success/failure

/**
 * WebSocketClient - A client for WebSocket communication using cpprestsdk
 */
class WebSocketClient {
public:
    WebSocketClient(const std::string& host, 
                   const std::string& port,
                   const std::string& path, 
                   const std::string& authToken);
    
    ~WebSocketClient();
    
    bool connect();
    bool send_message(const std::string& message, bool binary = false);
    bool send_binary_message(const std::vector<uint8_t>& data);
    void set_message_callback(MessageCallback callback);
    void set_connection_callback(ConnectionCallback callback);
    void close();
    bool is_connected() const;
    
private:
    web::uri uri_;
    std::unique_ptr<web::websockets::client::websocket_client> client_;
    bool is_connected_;
    MessageCallback message_callback_;
    ConnectionCallback connection_callback_;
};

// Connect to calculation service WebSocket endpoint using JWT token
std::shared_ptr<WebSocketClient> connectToCalculateWebSocket(
    const std::string& jwtToken,
    const std::string& host = "localhost",
    const std::string& port = "8080",
    MessageCallback messageCallback = nullptr,
    ConnectionCallback connectionCallback = nullptr
);

// Connect to calculation unit
std::shared_ptr<WebSocketClient> connectToCalculationUnit(
    const std::string& baseUrl, 
    const std::string& bearerToken,
    std::function<void(const std::string&, bool)> messageHandler = nullptr,
    ConnectionCallback connectionHandler = nullptr
);

// Send a MessagePack formatted message to the calculation unit
bool sendMessagePackToCalculationUnit(
    std::shared_ptr<WebSocketClient> client,
    const std::vector<uint8_t>& messagePackData
);

// Close the WebSocket connection to the calculation unit
void closeCalculationUnitConnection(std::shared_ptr<WebSocketClient> client);

#endif // WEBSOCKET_ADAPTER_H
