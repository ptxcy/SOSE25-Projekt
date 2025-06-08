#ifndef SCRIPT_COMMANDCENTER_HEADER
#define SCRIPT_COMMANDCENTER_HEADER


#include "../core/input.h"
#include "../core/wheel.h"


// zoom
constexpr f32 CMDSYS_ZOOM_ACCELLERATION = -.15f;
constexpr f32 CMDSYS_ZOOM_FLOATFACTOR = .95f;
constexpr f32 CMDSYS_ZOOM_MINDIST = 1.f;
constexpr f32 CMDSYS_ZOOM_MAXDIST = 150.f;
constexpr f32 CMDSYS_ZOOM_MIDDIST = (CMDSYS_ZOOM_MAXDIST+CMDSYS_ZOOM_MINDIST)*.5f;
constexpr f32 CMDSYS_ZOOM_HALFDIST_INV = 1/((CMDSYS_ZOOM_MAXDIST-CMDSYS_ZOOM_MINDIST)*.5f);
constexpr f32 CMDSYS_ZOOM_MINPITCH = 20.f;
constexpr f32 CMDSYS_ZOOM_MAXPITCH = 80.f;
constexpr f32 CMDSYS_ZOOM_HALFPITCH = (CMDSYS_ZOOM_MAXPITCH-CMDSYS_ZOOM_MINPITCH)*.5f;

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
	f32 m_ZoomMomentum = .15f;
};


#endif
