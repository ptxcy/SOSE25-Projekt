#!/bin/bash
# Compile with more verbose output to identify any potential issues
g++ -std=c++14 compact_websocket.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lboost_system -lmsgpackc -o websocket_client && ./websocket_client
