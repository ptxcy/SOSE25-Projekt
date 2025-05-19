# WebSocket Response Handling Implementation

This document describes the implementation of response handling in the WebSocket client for the AuthProxy system.

## Overview

The WebSocket client now has improved capabilities for parsing and interpreting MessagePack-formatted responses from the server. We've implemented three different levels of response handling:

1. **Basic** - Simple binary data reception and display
2. **Improved** - Enhanced visualization and basic MessagePack parsing
3. **Advanced** - Detailed deserialization and type-specific handling

## Server Response Structure

Responses from the server follow this MessagePack structure:

```rust
ServerMessage {
    response_info: ResponseInfo,
    response_data: ServerResponse
}

ResponseInfo {
    client: ClientInfo,
    authproxy: AuthproxyInfo,
    server: ServerInfo
}
```

The `response_data` field contains one of several variant types, corresponding to the request that was made:

- `DummySpawned` - Response to a `SpawnDummy` request
- `ClientFPSSet` - Response to a `SetClientFPS` request
- `DummyVelocitySet` - Response to a `DummySetVelocity` request

## Implementation Details

### Basic Response Handling

The basic implementation in `compact_websocket.cpp` provides:

```cpp
// Read the response
ws.read(response_buffer);

// Display as hex and ASCII
std::cout << "Hex representation: ";
for (size_t i = 0; i < response_buffer.data().size(); ++i) {
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << (static_cast<int>(static_cast<const char*>(response_buffer.data().data())[i]) & 0xFF) << " ";
}
std::cout << std::dec << std::endl;

// ASCII representation...
```

### Improved Response Handling

The improved implementation in `improved_websocket_client.cpp` adds:

```cpp
// Try to interpret as MessagePack
if (response_buffer.data().size() > 0) {
    try {
        // Create a zone for allocation
        msgpack::zone zone;

        // Deserialize MessagePack object
        msgpack::object obj = msgpack::unpack(
            static_cast<const char*>(response_buffer.data().data()),
            response_buffer.data().size(),
            nullptr,
            &zone
        ).get();

        std::cout << "MessagePack object type: " << obj.type << std::endl;
        std::cout << "MessagePack content: " << obj << std::endl;
    } catch (std::exception& e) {
        std::cout << "Failed to parse as MessagePack: " << e.what() << std::endl;
    }
}
```

### Advanced Response Handling

The advanced implementation in `advanced_websocket_client.cpp` adds:

```cpp
void analyze_response_data(const msgpack::object& obj) {
    if (obj.type != msgpack::type::MAP || obj.via.map.size == 0) {
        std::cout << "Response data is not a valid map or is empty\n";
        return;
    }

    // Look for response variants
    for (uint32_t i = 0; i < obj.via.map.size; ++i) {
        auto& kv = obj.via.map.ptr[i];
        if (kv.key.type == msgpack::type::STR) {
            std::string key_str(kv.key.via.str.ptr, kv.key.via.str.size);

            std::cout << "Response variant: " << key_str << std::endl;

            // Process different response types
            if (key_str == "DummySpawned") {
                // Extract DummySpawned specific fields...
            }
            else if (key_str == "ClientFPSSet") {
                // Extract ClientFPSSet specific fields...
            }
            else if (key_str == "DummyVelocitySet") {
                // Extract DummyVelocitySet specific fields...
            }
        }
    }
}
```

## Response Variants Processing

### DummySpawned

```cpp
// Try to extract the DummySpawned data
if (kv.val.type == msgpack::type::MAP) {
    std::string id;
    Coordinate position;

    // Manually extract fields from the map
    for (uint32_t j = 0; j < kv.val.via.map.size; ++j) {
        auto& field_kv = kv.val.via.map.ptr[j];
        if (field_kv.key.type == msgpack::type::STR) {
            std::string field_name(field_kv.key.via.str.ptr, field_kv.key.via.str.size);

            if (field_name == "id" && field_kv.val.type == msgpack::type::STR) {
                id = std::string(field_kv.val.via.str.ptr, field_kv.val.via.str.size);
            }
            else if (field_name == "position" && field_kv.val.type == msgpack::type::MAP) {
                // Extract position coordinates...
            }
        }
    }

    // Print out the extracted information
    std::cout << "Dummy spawned with ID: " << id << std::endl;
    std::cout << "Position: " << position << std::endl;
}
```

### ClientFPSSet

```cpp
try {
    double fps = 0;
    if (kv.val.type == msgpack::type::FLOAT) {
        fps = kv.val.via.f64;
    } else if (kv.val.type == msgpack::type::POSITIVE_INTEGER) {
        fps = static_cast<double>(kv.val.via.u64);
    } else if (kv.val.type == msgpack::type::NEGATIVE_INTEGER) {
        fps = static_cast<double>(kv.val.via.i64);
    }

    std::cout << "Client FPS set to: " << fps << std::endl;
} catch (std::exception& e) {
    std::cout << "Error parsing ClientFPSSet data: " << e.what() << std::endl;
}
```

## Useful Utilities

### Hex Dump Formatter

The advanced client includes a prettier hex dump formatter:

```cpp
void print_hex_dump(const char* data, size_t size) {
    std::cout << "Hex dump:" << std::endl;

    const size_t LINE_LENGTH = 16;
    for (size_t i = 0; i < size; i += LINE_LENGTH) {
        // Print offset
        std::cout << std::hex << std::setw(8) << std::setfill('0') << i << "  ";

        // Print hex values
        for (size_t j = 0; j < LINE_LENGTH; ++j) {
            // ...
        }

        // Print ASCII representation
        std::cout << " |";
        // ...
    }
}
```

### Logging with Timestamps

```cpp
class WebSocketLogger {
public:
    static void log_state(const std::string& message) {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count() % 1000;
        auto timer = std::time(nullptr);
        auto local_time = *std::localtime(&timer);

        std::cout << "["
                 << std::setw(2) << std::setfill('0') << local_time.tm_hour << ":"
                 << std::setw(2) << std::setfill('0') << local_time.tm_min << ":"
                 << std::setw(2) << std::setfill('0') << local_time.tm_sec << "."
                 << std::setw(3) << std::setfill('0') << ms
                 << "] " << message << std::endl;
    }
};
```

## Usage

The advanced client can be compiled and run using:

```bash
./run.sh advanced
```

This will allow you to select a message type to send, and then will properly interpret and display the server's response.
