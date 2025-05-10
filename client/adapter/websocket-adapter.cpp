#include "websocket-adapter.h"
#include <iostream>

using namespace web;
using namespace web::websockets::client;

// Now using message and connection callback types defined in the header

/**
 * WebSocketClient - A client for WebSocket communication using cpprestsdk
 *
 * This class handles establishing and managing WebSocket connections to
 * the calculation_unit service.
 */
class WebSocketClient {
public:
    WebSocketClient(const std::string& host, 
                   const std::string& port,
                   const std::string& path, 
                   const std::string& authToken)
        : uri_(U("ws://" + host + ":" + port + path)),
          is_connected_(false) {
        
        // Configure client with authentication header if provided
        websocket_client_config config;
        if (!authToken.empty()) {
            config.headers()[U("Authorization")] = U("Bearer " + authToken);
        }
        
        client_ = std::make_unique<websocket_client>(config);
    }
    
    ~WebSocketClient() {
        close();
    }
    
    // Connect to the WebSocket server
    bool connect() {
        try {
            // Connect to the server
            client_->connect(uri_).then([this](pplx::task<void> task) {
                try {
                    task.get(); // Will throw if there was an error
                    is_connected_ = true;
                    if (connection_callback_) {
                        connection_callback_(true);
                    }
                } catch (const std::exception& e) {
                    std::cerr << "WebSocket connection error: " << e.what() << std::endl;
                    if (connection_callback_) {
                        connection_callback_(false);
                    }
                    is_connected_ = false;
                }
            });
            
            // Set up message handler
            client_->set_message_handler([this](websocket_incoming_message message) {
                message.extract_string().then([this](utility::string_t data) {
                    if (message_callback_) {
                        bool is_binary = (message.message_type() == websocket_message_type::binary_message);
                        message_callback_(utility::conversions::to_utf8string(data), is_binary);
                    }
                });
            });
            
            return true;
        } catch (std::exception& e) {
            std::cerr << "WebSocket connection error: " << e.what() << std::endl;
            if (connection_callback_) {
                connection_callback_(false);
            }
            return false;
        }
    }
    
    // Send a message to the WebSocket server
    bool send_message(const std::string& message, bool binary = false) {
        try {
            if (!is_connected_) {
                std::cerr << "Not connected to WebSocket server" << std::endl;
                return false;
            }
            
            websocket_outgoing_message out_message;
            
            if (binary) {
                out_message.set_binary_message(
                    concurrency::streams::bytestream::open_istream(
                        std::vector<uint8_t>(message.begin(), message.end())
                    )
                );
            } else {
                out_message.set_utf8_message(message);
            }
            
            client_->send(out_message).wait();
            
            return true;
        } catch (std::exception& e) {
            std::cerr << "WebSocket write error: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Send a binary message (for MessagePack data)
    bool send_binary_message(const std::vector<uint8_t>& data) {
        try {
            if (!is_connected_) {
                std::cerr << "Not connected to WebSocket server" << std::endl;
                return false;
            }
            
            websocket_outgoing_message out_message;
            out_message.set_binary_message(
                concurrency::streams::bytestream::open_istream(data)
            );
            
            client_->send(out_message).wait();
            
            return true;
        } catch (std::exception& e) {
            std::cerr << "WebSocket binary write error: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Set a callback function to be executed when a message is received
    void set_message_callback(MessageCallback callback) {
        message_callback_ = std::move(callback);
    }
    
    // Set a callback for connection status changes
    void set_connection_callback(ConnectionCallback callback) {
        connection_callback_ = std::move(callback);
    }
    
    // Close the WebSocket connection
    void close() {
        if (!is_connected_) return;
        
        try {
            client_->close().wait();
            is_connected_ = false;
        } catch (std::exception& e) {
            std::cerr << "WebSocket close error: " << e.what() << std::endl;
        }
    }
    
    // Check if the client is connected
    bool is_connected() const {
        return is_connected_;
    }
    
private:
    web::uri uri_;
    std::unique_ptr<websocket_client> client_;
    bool is_connected_;
    MessageCallback message_callback_;
    ConnectionCallback connection_callback_;
};

/**
 * Connect to calculation service WebSocket endpoint using JWT token
 * 
 * @param jwtToken JWT token for authentication
 * @param host Server host (default: localhost)
 * @param port Server port (default: 8080)
 * @param messageCallback Callback function to handle received messages
 * @param connectionCallback Callback function to handle connection events
 * @return Shared pointer to the WebSocketClient instance
 */
std::shared_ptr<WebSocketClient> connectToCalculateWebSocket(
    const std::string& jwtToken,
    const std::string& host = "localhost",
    const std::string& port = "8080",
    MessageCallback messageCallback = nullptr,
    ConnectionCallback connectionCallback = nullptr
) {
    // Create WebSocket client for the /calculate endpoint
    auto client = std::make_shared<WebSocketClient>(host, port, "/calculate", jwtToken);
    
    // Set callbacks
    if (messageCallback) {
        client->set_message_callback(messageCallback);
    }
    
    if (connectionCallback) {
        client->set_connection_callback(connectionCallback);
    }
    
    // Connect to the server
    client->connect();
    
    return client;
}

/**
 * Creates a WebSocket connection to the calculation unit
 * 
 * @param baseUrl The base URL of the calculation unit (e.g., "localhost:8082")
 * @param bearerToken The Bearer token obtained from authentication
 * @param messageHandler A callback function to handle received messages
 * @param connectionHandler A callback function for connection status changes
 * @return A shared pointer to the WebSocket client
 */
std::shared_ptr<WebSocketClient> connectToCalculationUnit(
    const std::string& baseUrl, 
    const std::string& bearerToken,
    std::function<void(const std::string&, bool)> messageHandler = nullptr,
    ConnectionCallback connectionHandler = nullptr) {
    
    // Parse base URL to get host and port
    std::string host = baseUrl;
    std::string port = "80";  // Default port
    
    // Extract host and port from baseUrl
    size_t colonPos = baseUrl.find(':');
    if (colonPos != std::string::npos) {
        host = baseUrl.substr(0, colonPos);
        port = baseUrl.substr(colonPos + 1);
    }
    
    // Create WebSocket client
    auto client = std::make_shared<WebSocketClient>(
        host,
        port,
        "/msgpack",  // Path to the MessagePack endpoint
        bearerToken
    );
    
    // Set callback handlers
    if (messageHandler) {
        client->set_message_callback(messageHandler);
    }
    
    if (connectionHandler) {
        client->set_connection_callback(connectionHandler);
    }
    
    // Connect to the server
    client->connect();
    
    return client;
}

/**
 * Send a MessagePack formatted message to the calculation unit
 * 
 * @param client The WebSocket client connected to the calculation unit
 * @param messagePackData The MessagePack formatted data as a vector of bytes
 * @return True if the message was sent successfully, false otherwise
 */
bool sendMessagePackToCalculationUnit(
    std::shared_ptr<WebSocketClient> client,
    const std::vector<uint8_t>& messagePackData) {
    
    if (!client || !client->is_connected()) {
        std::cerr << "WebSocket client is not connected" << std::endl;
        return false;
    }
    
    return client->send_binary_message(messagePackData);
}

/**
 * Close the WebSocket connection to the calculation unit
 * 
 * @param client The WebSocket client connected to the calculation unit
 */
void closeCalculationUnitConnection(std::shared_ptr<WebSocketClient> client) {
    if (client) {
        client->close();
    }
}
