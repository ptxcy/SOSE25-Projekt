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
  - [Task](./task.md) - Contains detailed requirements for the implementation

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

- let the [run.sh](../adapter/run.sh) be as simple as possible and dont add prints or checks to it
