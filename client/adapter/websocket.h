#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <string>
#include <iomanip> // For hex output formatting
#include <mutex>
#include <thread>
#include <chrono>
#include <variant>
#include <map>
#include <sstream>
#include <condition_variable>
#include <ctime>
#include <functional>
#include <atomic>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <msgpack.hpp>

struct Coordinate
{
    double x;
    double y;
    double z;
    MSGPACK_DEFINE(x, y, z);
};

struct ClientInfo
{
    double sent_time = 0.0;
    MSGPACK_DEFINE(sent_time);
};

struct AuthproxyInfo
{
    double sent_time = 0.0;
    MSGPACK_DEFINE(sent_time);
};

struct RequestSyncInfo
{
    double sent_time = 0.0;
    MSGPACK_DEFINE(sent_time);
};

struct CalculationUnitInfo
{
    double sent_time = 0.0;
    MSGPACK_DEFINE(sent_time);
};

struct RequestInfo
{
    ClientInfo client;
    AuthproxyInfo authproxy;
    RequestSyncInfo request_sync;
    CalculationUnitInfo calculation_unit;
    MSGPACK_DEFINE(client, authproxy, request_sync, calculation_unit);
};

struct DummyObject
{
    std::string id;
    Coordinate position;
    Coordinate velocity;
    MSGPACK_DEFINE(id, position, velocity);
};

struct GameObjects
{
    std::map<std::string, DummyObject> dummies;
    MSGPACK_DEFINE(dummies);
};

struct ObjectData
{
    std::string target_user_id;
    GameObjects game_objects;
    MSGPACK_DEFINE(target_user_id, game_objects);
};

struct ServerMessage
{
    RequestInfo request_info;
    ObjectData request_data;
    MSGPACK_DEFINE(request_info, request_data);
};


// Message structures for client requests
struct SetClientFPS
{
    double fps;
    MSGPACK_DEFINE(fps);
};

struct SpawnDummy
{
    std::string id;
    MSGPACK_DEFINE(id);
};

struct DummySetVelocity
{
    std::string id;
    Coordinate position;
    MSGPACK_DEFINE(id, position);
};

struct ClientRequest
{
	std::optional<double> set_client_fps;
	std::optional<std::string> spawn_dummy;
	std::optional<DummySetVelocity> dummy_set_velocity;
	MSGPACK_DEFINE(set_client_fps,spawn_dummy,dummy_set_velocity);
};

struct ClientMessage
{
	RequestInfo request_info;
	ClientRequest request_data;
    MSGPACK_DEFINE(request_info,request_data);
};


// Global queues for server responses and outgoing messages
std::queue<ServerMessage> serverToClientMessage;
std::queue<ClientMessage> clientToServerMessage;

// Core function to start the WebSocket adapter
int startWebsocketAdapter();

// Functions to queue requests to the WebSocket server
void queueSpawnDummy(const std::string &dummy_id);
void queueSetClientFPS(double fps);
void queueDummySetVelocity(const std::string &dummy_id, double x, double y, double z);

#endif // WEBSOCKET_H
