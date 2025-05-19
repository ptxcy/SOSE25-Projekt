#!/bin/bash

# Choose which client implementation to compile and run
if [[ "$1" == "improved" ]]; then
    echo "Compiling and running improved WebSocket client..."
    g++ -std=c++14 improved_websocket_client.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lboost_system -o websocket_client && ./websocket_client
elif [[ "$1" == "advanced" ]]; then
    echo "Compiling and running advanced WebSocket client..."
    g++ -std=c++14 advanced_websocket_client.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lboost_system -o websocket_client && ./websocket_client
else
    echo "Compiling and running standard WebSocket client..."
    g++ -std=c++14 compact_websocket.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lboost_system -o websocket_client && ./websocket_client
fi
