#ifndef CORE_WEBSOCKET_HEADER
#define CORE_WEBSOCKET_HEADER


#include "base.h"
#include "../adapter/definition.h"


// Global queues for server responses and outgoing messages
std::queue<ServerMessage> serverToClientMessage;
std::queue<ClientMessage> clientToServerMessage;

// Core function to start the WebSocket adapter
void startWebsocketAdapter();


#endif
