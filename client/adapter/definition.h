#ifndef ADAPTER_DEFINITION_HEADER
#define ADAPTER_DEFINITION_HEADER


#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <msgpack.hpp>


// ----------------------------------------------------------------------------------------------------
// Information Data

struct Coordinate
{
	f64 x;
	f64 y;
	f64 z;
	MSGPACK_DEFINE(x,y,z);
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
	MSGPACK_DEFINE(client,authproxy,request_sync,calculation_unit);
};


// ----------------------------------------------------------------------------------------------------
// Server Messages

struct CraftingMaterial
{
	f64 copper;
	MSGPACK_DEFINE(copper);
};

struct Mine
{
	string owner;
	CraftingMaterial storage;
	MSGPACK_DEFINE(owner,storage);
};

struct Factory
{
	string owner;
	CraftingMaterial storage;
	MSGPACK_DEFINE(owner,storage);
};

struct BuildingRegion
{
	Coordinate relative_position;
	std::vector<Factory> factories;
	std::vector<Mine> mines;
	CraftingMaterial profit;
	MSGPACK_DEFINE(relative_position,factories,mines,profit);
};

struct DummyObject
{
	string owner;
	u64 id;
	string name;
	Coordinate position;
	Coordinate velocity;
	MSGPACK_DEFINE(owner,id,name,position,velocity);
};

struct Spacestation
{
	u64 parked;
	u64 capacity;
	MSGPACK_DEFINE(parked,capacity);
};

struct Planet
{
	string name;
	Coordinate position;
	std::vector<BuildingRegion> building_regions;
	f64 size;
	Spacestation spacestation;
	MSGPACK_DEFINE(name,position,building_regions,size,spacestation);
};

struct Player
{
	string username;
	f64 money;
	CraftingMaterial crafting_material;
	MSGPACK_DEFINE(username,money,crafting_material);
};

struct Spaceship
{
	u64 id;
	string owner;
	f64 speed;
	Coordinate velocity;
	Coordinate position;
	Coordinate target;
	bool docking_mode;
	std::optional<u64> docking_at;
	MSGPACK_DEFINE(id,owner,speed,velocity,position,target,docking_mode,docking_at);
};

struct GameObjects
{
	std::unordered_map<u64,DummyObject> dummies;
	std::vector<Planet> planets;
	std::unordered_map<string,Player> players;
	std::unordered_map<string,Spaceship> spaceships;
	MSGPACK_DEFINE(dummies,planets,players,spaceships);
};

struct ObjectData
{
	string target_user_id;
	GameObjects game_objects;
	MSGPACK_DEFINE(target_user_id,game_objects);
};

struct ServerMessage
{
	RequestInfo request_info;
	ObjectData request_data;
	MSGPACK_DEFINE(request_info,request_data);
};


// ----------------------------------------------------------------------------------------------------
// Client Messages

struct DummySetVelocity
{
	u64 id;
	Coordinate position;
	MSGPACK_DEFINE(id, position);
};

struct SetSpaceshipTarget
{
	u64 spaceship_id;
	u64 planet;
	MSGPACK_DEFINE(spaceship_id,planet);
};

struct ClientRequest
{
	std::optional<f64> set_client_fps;
	std::optional<string> spawn_dummy;
	std::optional<DummySetVelocity> dummy_set_velocity;
	std::optional<string> connect;
	std::optional<SetSpaceshipTarget> set_spaceship_target;
	std::optional<Coordinate> spawn_spaceship;
	std::optional<u64> delete_spaceship;
	MSGPACK_DEFINE(set_client_fps,spawn_dummy,dummy_set_velocity,connect,set_spaceship_target,
				   spawn_spaceship,delete_spaceship);
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
