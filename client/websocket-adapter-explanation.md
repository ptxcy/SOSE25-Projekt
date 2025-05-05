# WebSocket Adapter (`websocket-adapter.cpp`)

## Overview

This C++ file provides a WebSocket client implementation for communication with the `calculation_unit` service. It handles establishing authenticated WebSocket connections and sending/receiving both text and binary MessagePack data.

**Important Considerations:**

* **Pure ASIO Implementation:** This implementation uses the ASIO C++ library directly for WebSocket communication without requiring additional dependencies like Boost.Beast.
* **MessagePack Support:** Binary message support is included for sending MessagePack formatted data.
* **Authentication:** Supports Bearer token authentication as required by the AuthProxy service.
* **Asynchronous Communication:** Provides callback mechanisms for handling messages and connection status changes.
* **WebSocket Protocol:** Implements RFC6455 WebSocket protocol including proper framing, masking, and control frames.

## Key Components

### 1. `WebSocketClient` Class

```cpp
class WebSocketClient : public std::enable_shared_from_this<WebSocketClient> {
    // Methods and properties for managing WebSocket connections
};
```

A class that encapsulates WebSocket connection management with the following capabilities:

* **Connection Management:** Establishing, maintaining, and closing WebSocket connections.
* **Authentication:** Setting Bearer token in the Authorization header during connection handshake.
* **Message Exchange:** Sending text and binary messages, receiving messages asynchronously.
* **Protocol Handling:** Implementing WebSocket framing, masking, and control frame handling.
* **Event Handling:** Callback mechanisms for received messages and connection status changes.

### 2. Callback Types

```cpp
using MessageCallback = std::function<void(const std::string&, bool)>;
using ConnectionCallback = std::function<void(bool)>;
```

* `MessageCallback`: Function type for handling received WebSocket messages. The boolean parameter indicates whether the message is binary (true) or text (false).
* `ConnectionCallback`: Function type for handling connection status changes (connected/disconnected).

### 3. `connectToCalculationUnit` Function

```cpp
std::shared_ptr<WebSocketClient> connectToCalculationUnit(
    const std::string& baseUrl, 
    const std::string& bearerToken,
    std::function<void(const std::string&, bool)> messageHandler = nullptr,
    ConnectionCallback connectionHandler = nullptr);
```

* **Purpose**: Creates and establishes a WebSocket connection to the calculation unit.
* **Parameters**:
  * `baseUrl`: Host and port of the calculation unit (e.g., "localhost:8082").
  * `bearerToken`: Authentication token obtained from AuthProxy.
  * `messageHandler`: Optional callback for handling incoming messages.
  * `connectionHandler`: Optional callback for connection status updates.
* **Returns**: A shared pointer to the WebSocketClient if successful, nullptr otherwise.
* **Implementation**:
  * Parses the baseUrl to extract host and port.
  * Creates a WebSocket client instance with the appropriate parameters.
  * Sets up message and connection handlers if provided.
  * Connects to the server and starts the IO context in a separate thread.

### 4. `sendMessagePackToCalculationUnit` Function

```cpp
bool sendMessagePackToCalculationUnit(
    std::shared_ptr<WebSocketClient> client,
    const std::vector<uint8_t>& messagePackData);
```

* **Purpose**: Sends MessagePack formatted data to the calculation unit.
* **Parameters**:
  * `client`: The WebSocketClient connected to the calculation unit.
  * `messagePackData`: MessagePack formatted data as a vector of bytes.
* **Returns**: True if the message was sent successfully, false otherwise.

### 5. `closeCalculationUnitConnection` Function

```cpp
void closeCalculationUnitConnection(std::shared_ptr<WebSocketClient> client);
```

* **Purpose**: Gracefully closes the WebSocket connection to the calculation unit.
* **Parameters**:
  * `client`: The WebSocketClient to close.

## WebSocket Protocol Implementation

The adapter implements the WebSocket protocol according to RFC6455, including:

1. **Handshake**: The initial HTTP-based handshake with the server, including Authentication headers.
2. **Framing**: Proper framing of outgoing messages with FIN bit, opcode, masking, and payload.
3. **Control Frames**: Handling of WebSocket control frames:
   * **Close (0x8)**: For connection termination
   * **Ping (0x9)**: Automatically responds with Pong frames
   * **Pong (0xA)**: Processes incoming Pong frames
4. **Masking**: Client-side masking of data as required by the protocol.
5. **Binary Data**: Support for both text and binary messages (opcode 0x1 and 0x2, respectively).

## Connection Flow

The connection flow with the WebSocket adapter follows these steps:

1. **Authentication with AuthProxy**: The client first authenticates with the AuthProxy service using the HTTP adapter and obtains a Bearer token.

2. **WebSocket Connection**: The client then establishes a WebSocket connection to the calculation unit using the Bearer token:
   ```cpp
   auto wsClient = connectToCalculationUnit(
       "localhost:8082",      // Calculation unit address
       bearerToken,           // Token from authentication
       [](const std::string& msg, bool isBinary) {
           // Handle incoming messages
       },
       [](bool connected) {
           // Handle connection status changes
       }
   );
   ```

3. **Message Exchange**: Once connected, the client can send MessagePack formatted data:
   ```cpp
   std::vector<uint8_t> msgpackData = /* MessagePack formatted data */;
   sendMessagePackToCalculationUnit(wsClient, msgpackData);
   ```

4. **Connection Termination**: When done, the client can close the connection:
   ```cpp
   closeCalculationUnitConnection(wsClient);
   ```

## Dependencies

* **ASIO C++ Library**: For networking functionality (already included in the project).
* **Standard C++ Libraries**: string, iostream, memory, functional, vector, thread.

## Error Handling

* The WebSocket adapter includes comprehensive error handling for connection failures, read/write errors, and disconnection events.
* All errors are logged to stderr with descriptive messages.
* Connection callbacks are triggered appropriately to notify the application of status changes.

## Thread Safety

* The WebSocket communication occurs in a separate thread managed by ASIO's io_context.
* Callbacks are invoked from this thread, so appropriate synchronization may be needed when interacting with UI components.

## Usage Example

```cpp
// Example workflow:
// 1. First authenticate using the HTTP adapter
auto authResponse = authenticateUser(baseUrl, username, password);
std::string bearerToken = authResponse.authorizationHeader;

// 2. Create a lobby (if needed)
auto lobbyResponse = createLobby(baseUrl, lobbyName, bearerToken);

// 3. Connect to the calculation unit
auto wsClient = connectToCalculationUnit(
    "localhost:8082", 
    bearerToken,
    // Message handler
    [](const std::string& message, bool isBinary) {
        if (isBinary) {
            std::cout << "Received binary MessagePack data" << std::endl;
            // Handle MessagePack binary data
        } else {
            std::cout << "Received text: " << message << std::endl;
        }
    },
    // Connection handler
    [](bool connected) {
        if (connected) {
            std::cout << "Connected to calculation unit" << std::endl;
        } else {
            std::cout << "Disconnected from calculation unit" << std::endl;
        }
    }
);

// 4. Send MessagePack formatted data
std::vector<uint8_t> messagePackData = {/* MessagePack data */};
sendMessagePackToCalculationUnit(wsClient, messagePackData);

// 5. Later, when done with the connection
closeCalculationUnitConnection(wsClient);
```