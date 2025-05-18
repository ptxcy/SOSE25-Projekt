# WebSocket Client Implementation Task

> **Project Context:** This task is part of a larger project to implement adapters for an AuthProxy system. See the [complete feature request](./formatted_ticket.md) for the full context and additional requirements.

## Objective

Create a WebSocket client that sends messages in the MessagePack format to a server.

## Requirements

1. **Connection Type**: WebSocket client
2. **Server Endpoint**: localhost:8082/msgpack
3. **Message Format**: MessagePack
4. **Initial Implementation**: Start by sending a simple string message

## Technical Details

- Implement the client in C++ (as indicated by the websocket.cpp file)
- Establish a WebSocket connection to the specified endpoint
- Convert messages to MessagePack format before sending
- Handle connection establishment and message transmission

## Getting Started

- You may need to use a WebSocket library for C++ (e.g., websocketpp, Beast)
- A MessagePack library will be required (e.g., msgpack-c)
- Begin with a simple implementation that connects and sends a basic string message

## Relationship to Project

This specific task focuses on implementing the WebSocket client that formats messages using MessagePack. As part of the broader project (described in the [feature request](./formatted_ticket.md)), this component will be integrated with authorization mechanisms to allow authenticated connections to the AuthProxy system.

The current simplified task (connecting to localhost:8082/msgpack) serves as a starting point before implementing the full authentication flow described in the feature request.
