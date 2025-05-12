# WebSocket Adapter Implementation

This document provides an explanation of the WebSocket adapter implementation in the SOSE25-Projekt client adapter module.

## Overview

The `websocket-adapter.cpp` file implements a WebSocket client that enables real-time bidirectional communication between the C++ client application and backend microservices. It uses the C++ REST SDK (Casablanca) for handling WebSocket connections and provides a simplified interface for establishing connections, sending messages, and receiving incoming data.

## Class Structure

### `WebSocketClient` Class

The primary class that encapsulates WebSocket functionality:

```cpp
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
    // Private member variables...
};
```

## Key Components

### 1. Connection Management

The WebSocket client manages connections with the following methods:

- **Constructor**: Sets up the WebSocket URI with the necessary parameters (host, port, path, and optional authentication token).
- **`connect()`**: Establishes the connection to the WebSocket server and starts a background task for receiving messages.
- **`close()`**: Gracefully closes the WebSocket connection.
- **`is_connected()`**: Returns the current connection state.

### 2. Message Handling

The client supports both text and binary message formats:

- **`send_message()`**: Sends a text or binary message to the server.
- **`send_binary_message()`**: Specifically for sending binary data (like MessagePack serialized objects).
- **Message callbacks**: Allows registering callbacks to handle incoming messages.

### 3. Callback System

Two types of callbacks are supported:

- **`MessageCallback`**: Function called when a message is received from the server.
  ```cpp
  using MessageCallback = std::function<void(const std::string&, bool)>;
  ```
  The parameters represent the message content and a boolean indicating if it's a binary message.

- **`ConnectionCallback`**: Function called when the connection state changes.
  ```cpp
  using ConnectionCallback = std::function<void(bool)>;
  ```
  The boolean parameter indicates if the client is connected (true) or disconnected (false).

## Helper Functions

### MessagePack Integration

```cpp
bool sendMessagePack(
    std::shared_ptr<WebSocketClient> client,
    const std::vector<uint8_t>& messagePackData
)
```

This function simplifies sending MessagePack serialized data through the WebSocket connection. It checks if the client is valid and connected before sending the binary message.

### AuthProxy Connection

```cpp
std::shared_ptr<WebSocketClient> connectToAuthProxyWebSocket(
    const std::string& jwtToken,
    const std::string& host = "localhost",
    const std::string& port = "8080",
    const std::string& path = "/api/ws",
    MessageCallback messageCallback = nullptr,
    ConnectionCallback connectionCallback = nullptr
)
```

This helper function creates a WebSocket client specifically for connecting to the AuthProxy service. It:
1. Creates a client instance
2. Sets up the provided callbacks
3. Attempts to connect
4. Returns the client if connection was successful, or `nullptr` otherwise

## Error Handling

The implementation includes robust error handling:

- Every operation that could fail includes try-catch blocks to handle exceptions.
- Connection failures are reported through the connection callback.
- Errors during message sending or receiving are logged to standard error.

## Thread Safety

The WebSocket adapter runs message receiving in a separate thread, ensuring that:

1. The main application is not blocked while waiting for messages.
2. Messages can be received asynchronously even while the application is performing other tasks.

## Usage Example

```cpp
// Create and set up a WebSocket client
auto client = connectToAuthProxyWebSocket(
    jwtToken,
    "localhost",
    "8080",
    "/api/ws",
    [](const std::string& data, bool is_binary) {
        // Handle incoming messages
    },
    [](bool connected) {
        // Handle connection state changes
    }
);

// Send a message
client->send_message("Hello, server!");

// Send binary data
std::vector<uint8_t> binaryData = {...};
client->send_binary_message(binaryData);

// Send MessagePack data
auto packed_data = msgpack_helper::pack(some_object);
sendMessagePack(client, packed_data);

// Close the connection when done
client->close();
```

## Important Implementation Details

1. **Connection URI**: The WebSocket URI is constructed with the scheme `ws://`, plus the provided host, port, path, and optionally the authentication token as a query parameter.

2. **Message Reception**: Messages are received in a continuously running task that processes incoming data until the connection is closed.

3. **Connection Status**: The client maintains an internal flag (`is_connected_`) to track connection status.

## Dependencies

- **C++ REST SDK**: For WebSocket client implementation
- **PPL Tasks**: For asynchronous operations
- **MessagePack Helper**: For serializing/deserializing MessagePack data

## Potential Issue - Double Connection

When using the `connectToAuthProxyWebSocket()` helper function, the connection is established automatically within the function. Be cautious not to call `client->connect()` again after this function returns, as it would create a duplicate connection attempt.

```cpp
// CORRECT:
auto client = connectToAuthProxyWebSocket(...);
if (client) {
    // Already connected, just use it
    client->send_message("Hello");
}

// INCORRECT:
auto client = connectToAuthProxyWebSocket(...);
if (client) {
    client->connect(); // Don't do this - connection already established!
    client->send_message("Hello");
}
```
