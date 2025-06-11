#ifndef SCRIPT_COMMANDCENTER_HEADER
#define SCRIPT_COMMANDCENTER_HEADER


#include "../core/input.h"
#include "../core/wheel.h"
#include "webcomm.h"


// zoom
constexpr f32 CMDSYS_ZOOM_ACCELLERATION = -.15f;
constexpr f32 CMDSYS_ZOOM_FLOATFACTOR = .9f;
constexpr f32 CMDSYS_ZOOM_MINDIST = 2.f;
constexpr f32 CMDSYS_ZOOM_MAXDIST = 50.f;
constexpr f32 CMDSYS_ZOOM_EASE = .25f;

// yaw
constexpr f32 CMDSYS_ROT_MOUSEACC = -.05f;
constexpr f32 CMDSYS_ROT_KEYACC = .5f;
constexpr f32 CMDSYS_ROT_FLOATFACTOR = .8f;

// movement
constexpr f32 CMDSYS_MVMT_ACCELLERATION = .15f;
constexpr f32 CMDSYS_MVMT_FLOATFACTOR = .8f;


class CommandCenter
{
public:
	CommandCenter();
	void run();
	static inline void _update(void* cc) { CommandCenter* p = (CommandCenter*)cc; p->update(); }
	void update();
	// TODO better routine persistence through wheelcall also handling struct memory

private:

	// camera movement
	vec3 m_CameraMomentum = vec3(0);
	f32 m_ZoomMomentum = .3f;
	vec2 m_RotMomentum = vec2(.0f);
};


#endif
