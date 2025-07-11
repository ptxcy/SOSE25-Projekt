#ifndef SCRIPT_PONG_HEADER
#define SCRIPT_PONG_HEADER
#ifdef PROJECT_PONG


#include "../core/renderer.h"
#include "../core/input.h"
#include "../core/wheel.h"
#include "../core/websocket.h"
#include "../adapter/definition.h"
#include "webcomm.h"


// field constants
constexpr vec2 PONG_FIELD_SIZE = vec2(192,108);
constexpr f32 PONG_FIELD_TEXEL = PONG_FIELD_SIZE.x/4.f;
constexpr f32 PONG_FIELD_WIDTH = .15f;

// ball constants
constexpr u32 PONG_BALL_COUNT = 50*50;

// pedal constants
constexpr f32 PONG_PEDAL_ACCELERATION = .4f;
// FIXME do this through input upload and sync with server

// lightting constants
constexpr u32 PONG_LIGHTING_POINTLIGHTS = 16;


struct BallIndex
{
	vec3 position = vec3(0);
	f32 scale = 1.f;
};

class Pong
{
public:
	Pong(Font* font,string name);
	static inline void _update(void* rp) { Pong* p = (Pong*)rp; p->update(); }
	void update();

private:

	// renderer
	lptr<GeometryBatch> m_PhysicalBatch;
	lptr<ParticleBatch> m_BallBatch;

	// lighting
	PointLight* m_Lights[PONG_LIGHTING_POINTLIGHTS];
	/*
	PointLight* m_Light0;
	PointLight* m_Light1;
	PointLight* m_Light2;
	*/

	// players
	/*
	u32 m_Player0;
	u32 m_Player1;
	vec3 m_PlayerPosition0 = vec3(-17,0,0);
	vec3 m_PlayerPosition1 = vec3(17,0,0);
	*/

	// ball information
	BallIndex m_BallIndices[PONG_BALL_COUNT];
	/*
	u32 m_Ball0;
	u32 m_Ball1;
	u32 m_Ball2;
	vec3 m_BallPosition0 = vec3(2,-2,0);
	vec3 m_BallPosition1 = vec3(-3,4,0);
	vec3 m_BallPosition2 = vec3(-4.5f,-7,0);
	*/

	// scoreboard
	lptr<Text> m_Score0;
	lptr<Text> m_Score1;
};


#endif
p#endif
