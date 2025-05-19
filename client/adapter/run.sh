g++ -std=c++17 websocket.cpp -I/opt/homebrew/include -I/usr/local/include -L$MSGPACK_PATH -L/opt/homebrew/lib -L/usr/local/lib -lboost_system -lpthread -o websocket_client
