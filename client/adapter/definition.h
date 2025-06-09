#ifndef ADAPTER_DEFINITION_HEADER
#define ADAPTER_DEFINITION_HEADER


#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <msgpack.hpp>


struct Coordinate
{
	f64 x;
	f64 y;
	f64 z;
	MSGPACK_DEFINE(x, y, z);
};
// TODO make single precision coordinates and transition to vector library

struct ClientInfo
{
	f64 sent_time = 0.0;
	MSGPACK_DEFINE(sent_time);
};

struct AuthproxyInfo
{
	f64 sent_time = 0.0;
	MSGPACK_DEFINE(sent_time);
};

struct RequestSyncInfo
{
	f64 sent_time = 0.0;
	MSGPACK_DEFINE(sent_time);
};

struct CalculationUnitInfo
{
	f64 sent_time = 0.0;
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
	string id;
	Coordinate position;
	Coordinate velocity;
	MSGPACK_DEFINE(id, position, velocity);
};

struct GameObjects
{
	std::map<string, DummyObject> dummies;
	MSGPACK_DEFINE(dummies);
};

struct ObjectData
{
	string target_user_id;
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
	string id;
	f64 fps;
	MSGPACK_DEFINE(id,fps);
};

struct SpawnDummy
{
	string id;
	MSGPACK_DEFINE(id);
};

struct DummySetVelocity
{
	string id;
	Coordinate position;
	MSGPACK_DEFINE(id, position);
};

struct ClientRequest
{
	std::optional<SetClientFPS> set_client_fps;
	std::optional<string> spawn_dummy;
	std::optional<DummySetVelocity> dummy_set_velocity;
	std::optional<string> connect;
	MSGPACK_DEFINE(set_client_fps,spawn_dummy,dummy_set_velocity,connect);
};
// FIXME heavy contender for worst datastructure in the entire universe

struct ClientMessage
{
	RequestInfo request_info;
	ClientRequest request_data;
	string username;
	MSGPACK_DEFINE(request_info,request_data,username);
};


#endif
