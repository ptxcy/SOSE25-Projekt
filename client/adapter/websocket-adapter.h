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

// Connect to authproxy WebSocket endpoint using JWT token
std::shared_ptr<WebSocketClient> connectToAuthProxyWebSocket(
    const std::string& jwtToken,
    const std::string& host = "localhost",
    const std::string& port = "8080",
    const std::string& path = "/api/ws",
    MessageCallback messageCallback = nullptr,
    ConnectionCallback connectionCallback = nullptr
);

// Helper function to send a MessagePack formatted message
bool sendMessagePack(
    std::shared_ptr<WebSocketClient> client,
    const std::vector<uint8_t>& messagePackData
);

#endif // WEBSOCKET_ADAPTER_H
