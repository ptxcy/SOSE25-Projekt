#ifndef ADAPTER_PONG_HEADER
#define ADAPTER_PONG_HEADER


#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <msgpack.hpp>


// ----------------------------------------------------------------------------------------------------
// Server Communication

struct Coordinate
{
	f64 x;
	f64 y;
	f64 z;
	MSGPACK_DEFINE(x,y,z);
};

struct Ball
{
	Coordinate position;
	Coordinate velocity;
	f64 radius;
	f64 bounciness;
	MSGPACK_DEFINE(position,velocity,radius,bounciness);
};

struct Line
{
	Coordinate a;
	Coordinate b;
	MSGPACK_DEFINE(a,b);
};

struct Player
{
	f64 speed;
	bool team;
	Coordinate velocity;
	Coordinate position;
	Line relative_lines;
	MSGPACK_DEFINE(speed,team,velocity,position,relative_lines);
};

struct Score
{
	u16 player1;
	u16 player2;
	MSGPACK_DEFINE(player1,player2);
};

struct GameObject
{
	vector<Ball> balls;
	vector<Line> lines;
	map<string,Player> players;
	Score score;
	MSGPACK_DEFINE(balls,lines,players,score);
};

struct ObjectData
{
	string target_user_id;
	vector<u8> game_objects;
	MSGPACK_DEFINE(target_user_id,game_objects);
};

struct ServerMessage
{
	ObjectData request_data;
	MSGPACK_DEFINE(request_data);
};


// ----------------------------------------------------------------------------------------------------
// Client Communication

struct RequestData
{
	bool connect;
	s8 move_to;
	MSGPACK_DEFINE(connect,move_to);
};

struct ClientMessage
{
	RequestData request_data;
	string username;
	MSGPACK_DEFINE(request_data,username);
};


#endif
