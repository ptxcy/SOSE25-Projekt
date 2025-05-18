# SOSE25-Projekt Copilot Instructions

## Project Overview

This project involves implementing a WebSocket client adapter that sends MessagePack formatted data to a server endpoint. The client is written in C++ and needs to establish a connection to localhost:8082/msgpack. For detailed task requirements, see [task.md](./task.md).

## Technical Context

- **Language**: C++
- **Communication Protocol**: WebSocket
- **Data Format**: MessagePack
- **Target Endpoint**: localhost:8082/msgpack (already running and waiting for messages)

## Simplicity Focus

This implementation should prioritize simplicity and clarity:

- Focus on sending a single message in MessagePack format
- Keep the code minimal and straightforward
- Avoid complex error handling or edge cases
- Ensure the core functionality works with minimal dependencies
- Prefer a clean, short implementation over comprehensive features
- Do not create a server simulation as there is already an endpoint running at localhost:8082/msgpack waiting for messages

## Project Structure

- `adapter/` - Contains the WebSocket client implementation
  - `websocket.cpp` - Main implementation file for the WebSocket client
  - `task.md` - Contains detailed requirements for the implementation

## Coding Conventions

- Use modern C++ features (C++14 or later)
- Follow consistent naming conventions:
  - Class names: PascalCase
  - Functions/methods: camelCase
  - Variables: camelCase
  - Constants: UPPER_SNAKE_CASE
- Document code with clear comments
- Implement proper error handling

## Libraries and Dependencies

- Preferred WebSocket library: websocketpp or Boost.Beast
- MessagePack library: msgpack-c

## Implementation Requirements

1. Create a minimal WebSocket client that connects to the specified endpoint
2. Implement functionality to serialize a simple string using MessagePack format
3. Send a single MessagePack formatted message over the WebSocket connection
4. Keep error handling to the bare minimum

## Development Flow

1. Set up a minimal WebSocket client structure
2. Implement simple connection establishment to the endpoint
3. Add MessagePack serialization for a basic string message
4. Test sending a single message to verify functionality

## Notes for Copilot

When working with this project:

- Suggest appropriate include statements for required libraries
- Provide implementation patterns for WebSocket connection management
- Offer examples of MessagePack serialization for various data types
- Include error handling in suggested code blocks
- let the [run.sh](../adapter/run.sh) be as simple as possible and dont add prints or checks to it

## Code Examples

### Simple Implementation Example
```cpp
// websocket.cpp
#include <iostream>
#include <string>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>
#include <msgpack.hpp>

class WebSocketClient {
private:
    typedef websocketpp::client<websocketpp::config::asio_client> Client;
    
    Client client;
    websocketpp::connection_hdl connection;
    std::string serverEndpoint;

public:
    WebSocketClient(const std::string& endpoint);
    
    bool connect();
    bool sendMessage(const std::string& message);
    
private:
    void onOpen(websocketpp::connection_hdl hdl);
};
```

### Simple Connection Example
```cpp
WebSocketClient::WebSocketClient(const std::string& endpoint)
    : serverEndpoint(endpoint) {
    
    // Disable logging for simplicity
    client.clear_access_channels(websocketpp::log::alevel::all);
    
    // Initialize ASIO
    client.init_asio();
    
    // Register only the essential handler
    client.set_open_handler(std::bind(&WebSocketClient::onOpen, this, std::placeholders::_1));
}

bool WebSocketClient::connect() {
    try {
        // Create connection to the server
        websocketpp::lib::error_code ec;
        Client::connection_ptr con = client.get_connection(serverEndpoint, ec);
        
        // Connect to the server
        client.connect(con);
        
        // Start ASIO io_service run loop in a separate thread
        std::thread([this]() {
            this->client.run();
        }).detach();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
        return false;
    }
}
```

### Simple MessagePack Serialization Example
```cpp
bool WebSocketClient::sendMessage(const std::string& message) {
    try {
        // Create msgpack buffer
        msgpack::sbuffer buffer;
        msgpack::packer<msgpack::sbuffer> packer(buffer);
        
        // Pack the string
        packer.pack(message);
        
        // Send the serialized data as binary
        client.send(connection, buffer.data(), buffer.size(), 
                   websocketpp::frame::opcode::binary);
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}
```

### Callback Handler Example
```cpp
void WebSocketClient::onOpen(websocketpp::connection_hdl hdl) {
    connection = hdl;
    std::cout << "Connection established" << std::endl;
}
```


### Main Function Example
```cpp
int main() {
    // Create and connect client
    WebSocketClient client("ws://localhost:8082/msgpack");
    
    if (!client.connect()) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }
    
    // Send a simple string message
    client.sendMessage("Hello, MessagePack!");
    
    return 0;
}
```