#ifndef SCRIPT_PONG_HEADER
#define SCRIPT_PONG_HEADER


#include "../core/renderer.h"
#include "../core/input.h"
#include "../core/wheel.h"
#include "../core/websocket.h"
#include "../adapter/pong_adapter.h"
#include "webcomm.h"


constexpr vec2 PONG_FIELD_SIZE = vec2(20,10);
constexpr f32 PONG_FIELD_WIDTH = .15f;
constexpr f32 PONG_PEDAL_ACCELERATION = .4f;


class Pong
{
public:
	Pong(string name);
	static inline void _update(void* rp) { Pong* p = (Pong*)rp; p->update(); }
	void update();

private:

	// renderer
	lptr<GeometryBatch> m_FreeformBatch;
	lptr<GeometryBatch> m_PhysicalBatch;

	// lighting
	PointLight* m_Light0;
	PointLight* m_Light1;
	PointLight* m_Light2;

	// players
	u32 m_Player0;
	u32 m_Player1;
	vec3 m_PlayerPosition0 = vec3(-17,0,0);
	vec3 m_PlayerPosition1 = vec3(17,0,0);

	// ball information
	u32 m_Ball0;
	u32 m_Ball1;
	u32 m_Ball2;
	vec3 m_BallPosition0 = vec3(2,-2,0);
	vec3 m_BallPosition1 = vec3(-3,4,0);
	vec3 m_BallPosition2 = vec3(-4.5f,-7,0);
	// TODO make this dynamic
};


#endif
