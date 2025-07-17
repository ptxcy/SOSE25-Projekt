#ifndef SCRIPT_PONG_HEADER
#define SCRIPT_PONG_HEADER
#ifdef PROJECT_PONG


#include "../core/renderer.h"
#include "../core/input.h"
#include "../core/wheel.h"
#include "../core/websocket.h"
#include "../adapter/definition.h"
#include "webcomm.h"


// context constants
constexpr f32 PONG_SCALE_FACTOR = .1f;

// lighting constants
constexpr u32 PONG_LIGHTING_POINTLIGHTS = 64;

// ball constants
constexpr u32 PONG_BALL_COLUMN = 150;
constexpr u32 PONG_BALL_COUNT = PONG_BALL_COLUMN*PONG_BALL_COLUMN;
constexpr u32 PONG_BALL_PHYSICAL_COUNT = PONG_BALL_COUNT-PONG_LIGHTING_POINTLIGHTS;
constexpr f32 PONG_BALL_RADIUS = 2.f*PONG_SCALE_FACTOR;

// field constants
constexpr vec2 PONG_FIELD_SIZE = vec2(1920,1080)/2.2f*PONG_SCALE_FACTOR;
constexpr f32 PONG_FIELD_TEXEL = PONG_FIELD_SIZE.x/4.f;
constexpr f32 PONG_FIELD_WIDTH = .15f;
constexpr f32 PONG_FIELD_DEPTH = PONG_BALL_RADIUS+PONG_FIELD_WIDTH;


struct BallIndex
{
	vec3 position = vec3(0);
	f32 scale = PONG_BALL_RADIUS;
	vec3 colour = vec3(1);
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
	lptr<ParticleBatch> m_BulbBatch;

	// lighting
	PointLight* m_Lights[PONG_LIGHTING_POINTLIGHTS];

	// players
	u32 m_Player0;
	u32 m_Player1;
	bool m_Moving = false;

	// ball information
	BallIndex m_BallIndices[PONG_BALL_COUNT];
	BallIndex m_BulbIndices[PONG_LIGHTING_POINTLIGHTS];

	// scoreboard
	lptr<Text> m_Score0;
	lptr<Text> m_Score1;

	// measurements
	lptr<Text> m_FPS;
};


#endif
#endif
