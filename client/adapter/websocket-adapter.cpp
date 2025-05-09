#include <string>
#include <iostream>
#include <memory>
#include <functional>
#include <vector>
#include <thread>
#include <asio.hpp>
#include <asio/buffer.hpp>
#include <asio/error.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/streambuf.hpp>

// Forward declaration of message handlers
class WebSocketClient;
using MessageCallback = std::function<void(const std::string&, bool)>;
using ConnectionCallback = std::function<void(bool)>; // Bool indicates success/failure

/**
 * WebSocketClient - A client for WebSocket communication
 * 
 * This class handles establishing and managing WebSocket connections to
 * the calculation_unit service.
 */
class WebSocketClient : public std::enable_shared_from_this<WebSocketClient> {
public:
    WebSocketClient(const std::string& host, 
                    const std::string& port,
                    const std::string& path, 
                    const std::string& authToken)
        : host_(host),
          port_(port),
          path_(path),
          authToken_(authToken),
          io_context_(),
          socket_(io_context_),
          is_connected_(false),
          close_requested_(false) {
    }
    
    ~WebSocketClient() {
        close();
    }
    
    // Connect to the WebSocket server
    bool connect() {
        try {
            // Resolve the host
            asio::ip::tcp::resolver resolver(io_context_);
            auto endpoints = resolver.resolve(host_, port_);
            
            // Connect to the server
            asio::connect(socket_, endpoints);
            
            // Perform WebSocket handshake
            performWebSocketHandshake();
            
            is_connected_ = true;
            if (connection_callback_) {
                connection_callback_(true);
            }
            
            // Start reading messages
            startReadingMessages();
            
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
            
            // Create WebSocket frame
            std::vector<uint8_t> frame = createWebSocketFrame(message, binary);
            
            // Send the frame
            asio::write(socket_, asio::buffer(frame.data(), frame.size()));
            
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
            
            // Create WebSocket frame for binary data
            std::vector<uint8_t> frame = createWebSocketFrame(
                std::string(reinterpret_cast<const char*>(data.data()), data.size()),
                true // binary = true
            );
            
            // Send the frame
            asio::write(socket_, asio::buffer(frame.data(), frame.size()));
            
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
        
        close_requested_ = true;
        try {
            // Send WebSocket close frame
            std::vector<uint8_t> closeFrame = {0x88, 0x02, 0x03, 0xE8}; // Close frame with status 1000 (normal closure)
            asio::write(socket_, asio::buffer(closeFrame));
            
            // Close the socket
            socket_.close();
            is_connected_ = false;
        } catch (std::exception& e) {
            std::cerr << "WebSocket close error: " << e.what() << std::endl;
        }
    }
    
    // Check if the client is connected
    bool is_connected() const {
        return is_connected_;
    }
    
    // Run the IO context in its own thread
    void run_io_context() {
        try {
            io_context_.run();
        } catch (std::exception& e) {
            std::cerr << "IO context exception: " << e.what() << std::endl;
            if (is_connected_ && !close_requested_) {
                is_connected_ = false;
                if (connection_callback_) {
                    connection_callback_(false);
                }
            }
        }
    }
    
private:
    // Perform the WebSocket handshake
    void performWebSocketHandshake() {
        // Generate a random key for the Sec-WebSocket-Key header
        std::string secWebSocketKey = generateWebSocketKey();
        
        // Construct the HTTP upgrade request
        std::string request = "GET " + path_ + " HTTP/1.1\r\n";
        request += "Host: " + host_ + ":" + port_ + "\r\n";
        request += "Upgrade: websocket\r\n";
        request += "Connection: Upgrade\r\n";
        request += "Sec-WebSocket-Key: " + secWebSocketKey + "\r\n";
        request += "Sec-WebSocket-Version: 13\r\n";
        
        // Add Bearer token if provided
        if (!authToken_.empty()) {
            request += "Authorization: Bearer " + authToken_ + "\r\n";
        }
        
        request += "\r\n";
        
        // Send the request
        asio::write(socket_, asio::buffer(request));
        
        // Read the response
        asio::streambuf response;
        asio::read_until(socket_, response, "\r\n\r\n");
        
        // Parse the response
        std::istream response_stream(&response);
        std::string http_version;
        response_stream >> http_version;
        
        unsigned int status_code;
        response_stream >> status_code;
        
        std::string status_message;
        std::getline(response_stream, status_message);
        
        // Check the status code
        if (status_code != 101) {
            throw std::runtime_error("WebSocket handshake failed with status: " + 
                                     std::to_string(status_code) + " " + status_message);
        }
        
        // Read and validate headers
        bool upgraded = false;
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {
            // Convert header to lowercase for case-insensitive comparison
            std::string lowercaseHeader = header;
            std::transform(lowercaseHeader.begin(), lowercaseHeader.end(), lowercaseHeader.begin(), ::tolower);
            
            if (lowercaseHeader.find("upgrade: websocket") != std::string::npos) {
                upgraded = true;
            }
        }
        
        if (!upgraded) {
            throw std::runtime_error("WebSocket handshake failed: Server did not agree to upgrade");
        }
    }
    
    // Generate a random WebSocket key
    std::string generateWebSocketKey() {
        // Generate 16 random bytes
        unsigned char randomBytes[16];
        for (int i = 0; i < 16; ++i) {
            randomBytes[i] = rand() % 256;
        }
        
        // Base64 encode the random bytes
        static const char base64Chars[] = 
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        
        std::string encoded;
        encoded.reserve(24); // Base64 encoding of 16 bytes is 24 characters
        
        for (int i = 0; i < 16; i += 3) {
            // Convert 3 bytes to 4 Base64 characters
            uint32_t triplet = (randomBytes[i] << 16);
            if (i + 1 < 16) triplet |= (randomBytes[i + 1] << 8);
            if (i + 2 < 16) triplet |= randomBytes[i + 2];
            
            encoded.push_back(base64Chars[(triplet >> 18) & 0x3F]);
            encoded.push_back(base64Chars[(triplet >> 12) & 0x3F]);
            encoded.push_back((i + 1 < 16) ? base64Chars[(triplet >> 6) & 0x3F] : '=');
            encoded.push_back((i + 2 < 16) ? base64Chars[triplet & 0x3F] : '=');
        }
        
        return encoded;
    }
    
    // Start reading messages from the WebSocket
    void startReadingMessages() {
        // Read in a loop
        readNextMessage();
    }
    
    // Read the next message from the WebSocket
    void readNextMessage() {
        auto self = shared_from_this();
        
        // First read the header (at least 2 bytes for a WebSocket frame)
        auto headerBuffer = std::make_shared<std::vector<uint8_t>>(2);
        asio::async_read(
            socket_,
            asio::buffer(*headerBuffer),
            [this, self, headerBuffer](asio::error_code ec, std::size_t bytes_transferred) {
                if (ec) {
                    handleReadError(ec);
                    return;
                }
                
                // Parse the header
                bool fin = ((*headerBuffer)[0] & 0x80) != 0;
                uint8_t opcode = (*headerBuffer)[0] & 0x0F;
                bool masked = ((*headerBuffer)[1] & 0x80) != 0;
                uint64_t payload_length = (*headerBuffer)[1] & 0x7F;
                
                // Check if this is a control frame
                if (opcode == 0x08) { // Close frame
                    handleCloseFrame();
                    return;
                } else if (opcode == 0x09) { // Ping frame
                    handlePingFrame(payload_length);
                    return;
                } else if (opcode == 0x0A) { // Pong frame
                    // Just read any payload and ignore it, then continue
                    handlePongFrame(payload_length);
                    return;
                }
                
                // Read extended payload length if needed
                if (payload_length == 126) {
                    auto extLenBuffer = std::make_shared<std::vector<uint8_t>>(2);
                    asio::async_read(
                        socket_,
                        asio::buffer(*extLenBuffer),
                        [this, self, opcode, masked, extLenBuffer](asio::error_code ec, std::size_t bytes_transferred) {
                            if (ec) {
                                handleReadError(ec);
                                return;
                            }
                            
                            uint64_t actual_length = ((*extLenBuffer)[0] << 8) | (*extLenBuffer)[1];
                            readWebSocketPayload(opcode, masked, actual_length);
                        });
                } else if (payload_length == 127) {
                    auto extLenBuffer = std::make_shared<std::vector<uint8_t>>(8);
                    asio::async_read(
                        socket_,
                        asio::buffer(*extLenBuffer),
                        [this, self, opcode, masked, extLenBuffer](asio::error_code ec, std::size_t bytes_transferred) {
                            if (ec) {
                                handleReadError(ec);
                                return;
                            }
                            
                            uint64_t actual_length = 
                                (static_cast<uint64_t>((*extLenBuffer)[0]) << 56) |
                                (static_cast<uint64_t>((*extLenBuffer)[1]) << 48) |
                                (static_cast<uint64_t>((*extLenBuffer)[2]) << 40) |
                                (static_cast<uint64_t>((*extLenBuffer)[3]) << 32) |
                                (static_cast<uint64_t>((*extLenBuffer)[4]) << 24) |
                                (static_cast<uint64_t>((*extLenBuffer)[5]) << 16) |
                                (static_cast<uint64_t>((*extLenBuffer)[6]) << 8) |
                                static_cast<uint64_t>((*extLenBuffer)[7]);
                            
                            readWebSocketPayload(opcode, masked, actual_length);
                        });
                } else {
                    // Use the payload_length as is
                    readWebSocketPayload(opcode, masked, payload_length);
                }
            });
    }
    
    // Read the WebSocket payload data
    void readWebSocketPayload(uint8_t opcode, bool masked, uint64_t payload_length) {
        auto self = shared_from_this();
        
        // Read masking key if present
        if (masked) {
            auto maskBuffer = std::make_shared<std::vector<uint8_t>>(4);
            asio::async_read(
                socket_,
                asio::buffer(*maskBuffer),
                [this, self, opcode, payload_length, maskBuffer](asio::error_code ec, std::size_t bytes_transferred) {
                    if (ec) {
                        handleReadError(ec);
                        return;
                    }
                    
                    readPayloadData(opcode, payload_length, true, *maskBuffer);
                });
        } else {
            readPayloadData(opcode, payload_length, false, std::vector<uint8_t>());
        }
    }
    
    // Read the actual payload data
    void readPayloadData(uint8_t opcode, uint64_t payload_length, bool masked, const std::vector<uint8_t>& mask) {
        auto self = shared_from_this();
        
        if (payload_length > 0) {
            auto payloadBuffer = std::make_shared<std::vector<uint8_t>>(payload_length);
            asio::async_read(
                socket_,
                asio::buffer(*payloadBuffer),
                [this, self, opcode, masked, mask, payloadBuffer](asio::error_code ec, std::size_t bytes_transferred) {
                    if (ec) {
                        handleReadError(ec);
                        return;
                    }
                    
                    // Unmask the data if needed
                    if (masked) {
                        for (size_t i = 0; i < payloadBuffer->size(); ++i) {
                            (*payloadBuffer)[i] ^= mask[i % 4];
                        }
                    }
                    
                    // Process the message based on opcode
                    bool is_binary = (opcode == 0x02);
                    
                    // Call the message callback
                    if (message_callback_) {
                        std::string message(payloadBuffer->begin(), payloadBuffer->end());
                        message_callback_(message, is_binary);
                    }
                    
                    // Continue reading
                    readNextMessage();
                });
        } else {
            // Empty message
            if (message_callback_) {
                bool is_binary = (opcode == 0x02);
                message_callback_("", is_binary);
            }
            
            // Continue reading
            readNextMessage();
        }
    }
    
    // Handle a close frame
    void handleCloseFrame() {
        if (!close_requested_) {
            // Echo the close frame to properly close the connection
            close();
        }
    }
    
    // Handle a ping frame by responding with a pong
    void handlePingFrame(uint64_t payload_length) {
        auto self = shared_from_this();
        
        // Read the ping payload (if any)
        if (payload_length > 0) {
            auto pingPayload = std::make_shared<std::vector<uint8_t>>(payload_length);
            asio::async_read(
                socket_,
                asio::buffer(*pingPayload),
                [this, self, pingPayload](asio::error_code ec, std::size_t bytes_transferred) {
                    if (ec) {
                        handleReadError(ec);
                        return;
                    }
                    
                    // Create and send a pong frame with the same payload
                    sendPongFrame(*pingPayload);
                    
                    // Continue reading
                    readNextMessage();
                });
        } else {
            // Send an empty pong frame
            sendPongFrame(std::vector<uint8_t>());
            
            // Continue reading
            readNextMessage();
        }
    }
    
    // Send a pong frame in response to a ping
    void sendPongFrame(const std::vector<uint8_t>& payload) {
        try {
            // Create a pong frame
            std::vector<uint8_t> pongFrame;
            pongFrame.push_back(0x8A); // FIN bit set + opcode 0x0A for pong
            
            // Add payload length
            if (payload.size() <= 125) {
                pongFrame.push_back(static_cast<uint8_t>(payload.size()));
            } else if (payload.size() <= 65535) {
                pongFrame.push_back(126);
                pongFrame.push_back(static_cast<uint8_t>((payload.size() >> 8) & 0xFF));
                pongFrame.push_back(static_cast<uint8_t>(payload.size() & 0xFF));
            } else {
                // Unlikely for a ping frame, but handling for completeness
                pongFrame.push_back(127);
                for (int i = 7; i >= 0; --i) {
                    pongFrame.push_back(static_cast<uint8_t>((payload.size() >> (i * 8)) & 0xFF));
                }
            }
            
            // Add the payload
            pongFrame.insert(pongFrame.end(), payload.begin(), payload.end());
            
            // Send the pong
            asio::write(socket_, asio::buffer(pongFrame));
            
        } catch (std::exception& e) {
            std::cerr << "Error sending pong: " << e.what() << std::endl;
        }
    }
    
    // Handle a pong frame (usually just ignore it)
    void handlePongFrame(uint64_t payload_length) {
        auto self = shared_from_this();
        
        // Skip the payload
        if (payload_length > 0) {
            auto pongPayload = std::make_shared<std::vector<uint8_t>>(payload_length);
            asio::async_read(
                socket_,
                asio::buffer(*pongPayload),
                [this, self](asio::error_code ec, std::size_t bytes_transferred) {
                    if (ec) {
                        handleReadError(ec);
                        return;
                    }
                    
                    // Continue reading
                    readNextMessage();
                });
        } else {
            // Continue reading
            readNextMessage();
        }
    }
    
    // Handle read errors
    void handleReadError(const asio::error_code& ec) {
        if (!close_requested_) {
            std::cerr << "WebSocket read error: " << ec.message() << std::endl;
            is_connected_ = false;
            if (connection_callback_) {
                connection_callback_(false);
            }
        }
    }
    
    // Create a WebSocket frame from a message
    std::vector<uint8_t> createWebSocketFrame(const std::string& message, bool binary) {
        std::vector<uint8_t> frame;
        
        // First byte: FIN bit + opcode (0x01 for text, 0x02 for binary)
        frame.push_back(0x80 | (binary ? 0x02 : 0x01));
        
        // Second byte: Mask bit (client always masks) + payload length
        if (message.size() <= 125) {
            frame.push_back(0x80 | static_cast<uint8_t>(message.size()));
        } else if (message.size() <= 65535) {
            frame.push_back(0x80 | 126);
            frame.push_back(static_cast<uint8_t>((message.size() >> 8) & 0xFF));
            frame.push_back(static_cast<uint8_t>(message.size() & 0xFF));
        } else {
            frame.push_back(0x80 | 127);
            for (int i = 7; i >= 0; --i) {
                frame.push_back(static_cast<uint8_t>((message.size() >> (i * 8)) & 0xFF));
            }
        }
        
        // Masking key (4 bytes)
        uint8_t mask[4];
        for (int i = 0; i < 4; ++i) {
            mask[i] = rand() % 256;
            frame.push_back(mask[i]);
        }
        
        // Masked payload data
        for (size_t i = 0; i < message.size(); ++i) {
            frame.push_back(message[i] ^ mask[i % 4]);
        }
        
        return frame;
    }
    
    std::string host_;
    std::string port_;
    std::string path_;
    std::string authToken_;
    
    asio::io_context io_context_;
    asio::ip::tcp::socket socket_;
    
    bool is_connected_;
    bool close_requested_;
    
    MessageCallback message_callback_;
    ConnectionCallback connection_callback_;
};

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
        "/msgpack",  // Path to the MessagePack endpoint according to the task
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
    if (!client->connect()) {
        std::cerr << "Failed to connect to calculation unit at " << baseUrl << std::endl;
        return nullptr;
    }
    
    // Start IO context in separate thread
    std::thread([client]() {
        client->run_io_context();
    }).detach();
    
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

// redet nur mit authproxy
struct erstellen um msgpack zu senden
lib zum decoden und encoden von msgpack