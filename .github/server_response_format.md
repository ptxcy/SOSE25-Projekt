# Server Response Format Documentation

This document details the structure of responses received from the WebSocket server endpoint when using our MessagePack client.

## Actual Server Response Structure

Based on our testing and observation of server responses, we've identified the following Rust structure that appears to match the MessagePack data:

```rust
pub struct ServerMessage {
    pub request_info: RequestInfo,
    pub request_data: ObjectData,
}

pub struct ObjectData {
    pub target_user_id: String,
    pub game_objects: GameObjects,
}

pub struct GameObjects {
    pub dummies: HashMap<String, DummyObject>,
}

pub struct DummyObject {
    pub id: String,
    pub position: Coordinate,
    pub velocity: Coordinate,
}
```

## Implementation Notes

The current client implementation displays this data but does not fully parse the nested structure into C++ objects. To fully utilize this data, additional parsing logic would be needed to convert the array format into proper C++ objects representing the game state.

The client could be enhanced to maintain a local representation of the game state that gets updated with each response from the server.
