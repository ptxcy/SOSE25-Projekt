#ifndef SCRIPT_COMMANDCENTER_HEADER
#define SCRIPT_COMMANDCENTER_HEADER


#include "../core/input.h"
#include "../core/wheel.h"


constexpr f32 CMDSYS_ZOOM_ACCELLERATION = -.02f;
constexpr f32 CMDSYS_MVMT_ACCELLERATION = .01f;
constexpr f32 CMDSYS_MOMENTUM_FLOATFACTOR = .95f;


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
	vec3 m_CameraMomentum = vec3(0,.15f,0);
};


#endif
