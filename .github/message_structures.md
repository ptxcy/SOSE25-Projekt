# C++ Message Structures for WebSocket Client

This document provides C++ equivalents of the Rust message structures required for the WebSocket client implementation.

## Basic Structures

### Coordinate

```cpp
struct Coordinate {
    double x;
    double y;
    double z;
    
    // MessagePack serialization
    MSGPACK_DEFINE(x, y, z);
};
```

### Client Info

```cpp
struct ClientInfo {
    double sent_time;
    
    // MessagePack serialization
    MSGPACK_DEFINE(sent_time);
};
```

### AuthProxy Info

```cpp
struct AuthproxyInfo {
    double sent_time;
    
    // MessagePack serialization
    MSGPACK_DEFINE(sent_time);
};
```

### Request Sync Info

```cpp
struct RequestSyncInfo {
    double sent_time;
    
    // MessagePack serialization
    MSGPACK_DEFINE(sent_time);
};
```

### Calculation Unit Info

```cpp
struct CalculationUnitInfo {
    double sent_time;
    
    // MessagePack serialization
    MSGPACK_DEFINE(sent_time);
};
```

### Request Info

```cpp
struct RequestInfo {
    ClientInfo client;
    AuthproxyInfo authproxy;
    RequestSyncInfo request_sync;
    CalculationUnitInfo calculation_unit;
    
    // MessagePack serialization
    MSGPACK_DEFINE(client, authproxy, request_sync, calculation_unit);
};
```

## Request Types

In C++, we can represent the Rust enum `ClientRequest` in several ways.

### Approach: Using std::variant (more modern C++)

```cpp
// Individual request structures
struct SetClientFPS {
    double fps;
    
    MSGPACK_DEFINE(fps);
};

struct SpawnDummy {
    std::string id;
    
    MSGPACK_DEFINE(id);
};

struct DummySetVelocity {
    std::string id;
    Coordinate position;
    
    MSGPACK_DEFINE(id, position);
};

// Define the variant type
using ClientRequest = std::variant<SetClientFPS, SpawnDummy, DummySetVelocity>;
```

## Client Message

```cpp
struct ClientMessage {
    RequestInfo request_info;
    std::unique_ptr<ClientRequest> request_data;  // For Approach 1
    // or 
    // ClientRequest request_data;                // For Approach 2 (variant)
    
    // Custom serialization for MessagePack
    template <typename Packer>
    void msgpack_pack(Packer& pk) const {
        pk.pack_array(2);
        pk.pack(request_info);
        request_data->msgpack_pack(pk);  // For Approach 1
        
        // For Approach 2 (variant), you would use std::visit
        /*
        std::visit([&pk](const auto& req) {
            if constexpr (std::is_same_v<std::decay_t<decltype(req)>, SetClientFPS>) {
                pk.pack_array(2);
                pk.pack(0); // Type index for SetClientFPS
                pk.pack(req);
            }
            // Similar for other types...
        }, request_data);
        */
    }
};
```

## Helper Functions

```cpp
// Helper function to get current time
inline double getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return static_cast<double>(millis) / 1000.0;
}

// Helper function to create a SetClientFPS message
inline ClientMessage createSetClientFPSMessage(double fps) {
    double current_time = getCurrentTime();
    
    ClientInfo client_info{current_time};
    AuthproxyInfo authproxy_info{current_time};
    RequestSyncInfo request_sync_info{current_time};
    CalculationUnitInfo calculation_unit_info{current_time};
    
    RequestInfo request_info{
        client_info,
        authproxy_info,
        request_sync_info,
        calculation_unit_info
    };
    
    // For Approach 1
    ClientMessage message;
    message.request_info = request_info;
    message.request_data = std::make_unique<SetClientFPSRequest>(fps);
    return message;
    
    // For Approach 2
    /*
    return ClientMessage{
        request_info,
        SetClientFPS{fps}
    };
    */
}

// Similar helper functions for other message types
inline ClientMessage createSpawnDummyMessage(const std::string& id) {
    // Similar implementation to above
}

inline ClientMessage createDummySetVelocityMessage(
    const std::string& id, double x, double y, double z) {
    // Similar implementation to above
}
```

## Usage Example

```cpp
// Example of creating and serializing a message
void sendSetClientFPSMessage(WebSocketClient& client, double fps) {
    // Create the message
    auto message = createSetClientFPSMessage(fps);
    
    // Serialize with MessagePack
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, message);
    
    // Send via WebSocket
    client.send_binary(buffer.data(), buffer.size());
}
```

## Notes on Implementation

1. You'll need to include the appropriate headers:

   ```cpp
   #include <string>
   #include <memory>
   #include <variant>
   #include <chrono>
   #include <msgpack.hpp>
   ```

2. The MessagePack serialization uses the `MSGPACK_DEFINE` macro, which requires the msgpack-c library.

3. Choose between the polymorphic approach (Approach 1) or the variant approach (Approach 2) based on your preference and project requirements.

4. When implementing the actual client code, you'll need to integrate this with your WebSocket library (Beast, websocketpp, etc.) to send the serialized data over the connection.

5. Remember that the endpoint expects binary data, so make sure to set the WebSocket message type to binary before sending.
