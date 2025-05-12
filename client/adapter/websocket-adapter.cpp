#include "websocket-adapter.h"
#include "message-pack-helper.h"
#include <pplx/pplxtasks.h>
#include <cpprest/uri_builder.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <iostream>

using namespace web;
using namespace web::websockets::client;
using namespace utility;
using namespace concurrency;

// Implementation of WebSocketClient class
WebSocketClient::WebSocketClient(const std::string& host, 
                                const std::string& port,
                                const std::string& path, 
                                const std::string& authToken) 
    : is_connected_(false) {
    
    // Build WebSocket URI
    uri_builder builder;
    builder.set_scheme(U("ws"));
    builder.set_host(conversions::to_string_t(host));
    builder.set_port(conversions::to_string_t(port));
    builder.set_path(conversions::to_string_t(path));
    
    if (!authToken.empty()) {
        builder.append_query(U("token"), conversions::to_string_t(authToken));
    }
    
    uri_ = builder.to_uri();
    
    // Create client with custom configuration if needed
    websocket_client_config config;
    client_ = std::make_unique<websocket_client>(config);
}

WebSocketClient::~WebSocketClient() {
    if (is_connected_) {
        try {
            close();
        }
        catch (const std::exception& e) {
            std::cerr << "Error in WebSocketClient destructor: " << e.what() << std::endl;
        }
    }
}

bool WebSocketClient::connect() {
    try {
        client_->connect(uri_).wait();
        is_connected_ = true;
        
        if (connection_callback_) {
            connection_callback_(true);
        }
        
        // Create a task to receive messages continuously in a separate thread
        pplx::create_task([this]() {
            while (is_connected_) {
                try {
                    auto msg = client_->receive().get();
                    if (message_callback_) {
                        // Pass message content and binary flag to callback
                        message_callback_(
                            conversions::to_utf8string(msg.extract_string().get()),
                            msg.message_type() == websocket_message_type::binary
                        );
                    }
                }
                catch (const std::exception& e) {
                    if (is_connected_) {
                        std::cerr << "Error receiving WebSocket message: " << e.what() << std::endl;
                        is_connected_ = false;
                        if (connection_callback_) {
                            connection_callback_(false);
                        }
                        break;
                    }
                }
            }
        });
        
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to connect to WebSocket server: " << e.what() << std::endl;
        is_connected_ = false;
        if (connection_callback_) {
            connection_callback_(false);
        }
        return false;
    }
}

bool WebSocketClient::send_message(const std::string& message, bool binary) {
    if (!is_connected_) {
        std::cerr << "Cannot send message: WebSocket is not connected" << std::endl;
        return false;
    }
    
    try {
        websocket_message msg;
        if (binary) {
            // Create a binary message
            msg = websocket_message::create_binary_message(
                conversions::to_string_t(message)
            );
        } else {
            // Create a text message
            msg = websocket_message::create_message(
                conversions::to_string_t(message)
            );
        }
        
        client_->send(msg).wait();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to send WebSocket message: " << e.what() << std::endl;
        return false;
    }
}

bool WebSocketClient::send_binary_message(const std::vector<uint8_t>& data) {
    if (!is_connected_) {
        std::cerr << "Cannot send binary message: WebSocket is not connected" << std::endl;
        return false;
    }
    
    try {
        std::string binary_data(data.begin(), data.end());
        
        websocket_message msg = websocket_message::create_binary_message(
            conversions::to_string_t(binary_data)
        );
        
        client_->send(msg).wait();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to send WebSocket binary message: " << e.what() << std::endl;
        return false;
    }
}

void WebSocketClient::set_message_callback(MessageCallback callback) {
    message_callback_ = callback;
}

void WebSocketClient::set_connection_callback(ConnectionCallback callback) {
    connection_callback_ = callback;
}

void WebSocketClient::close() {
    if (is_connected_) {
        try {
            client_->close().wait();
        }
        catch (const std::exception& e) {
            std::cerr << "Error closing WebSocket connection: " << e.what() << std::endl;
        }
        is_connected_ = false;
    }
}

bool WebSocketClient::is_connected() const {
    return is_connected_;
}

// Helper function to send a MessagePack formatted message
bool sendMessagePack(
    std::shared_ptr<WebSocketClient> client,
    const std::vector<uint8_t>& messagePackData
) {
    if (!client || !client->is_connected()) {
        std::cerr << "Cannot send MessagePack: WebSocket client is not connected" << std::endl;
        return false;
    }
    
    return client->send_binary_message(messagePackData);
}

// Connect to authproxy WebSocket endpoint using JWT token
std::shared_ptr<WebSocketClient> connectToAuthProxyWebSocket(
    const std::string& jwtToken,
    const std::string& host = "localhost",
    const std::string& port = "8080",
    const std::string& path = "/api/ws",
    MessageCallback messageCallback = nullptr,
    ConnectionCallback connectionCallback = nullptr
) {
    auto client = std::make_shared<WebSocketClient>(host, port, path, jwtToken);
    
    if (messageCallback) {
        client->set_message_callback(messageCallback);
    }
    
    if (connectionCallback) {
        client->set_connection_callback(connectionCallback);
    }
    
    if (!client->connect()) {
        return nullptr;
    }
    
    return client;
}