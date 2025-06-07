#include "command_center.h"


/**
 *	command center setup
 */
CommandCenter::CommandCenter() {  }

/**
 *	TODO placeholder until memory management fixed and this is finally on heap
 */
void CommandCenter::run()
{
	g_Wheel.call(UpdateRoutine{ &CommandCenter::_update,(void*)this });
}

/**
 *	command center handling
 */
void CommandCenter::update()
{
	// camera movement
	m_CameraMomentum += vec3((g_Input.keyboard.keys[SDL_SCANCODE_D]-g_Input.keyboard.keys[SDL_SCANCODE_A]),
							 (g_Input.keyboard.keys[SDL_SCANCODE_W]-g_Input.keyboard.keys[SDL_SCANCODE_S]),
							 0)*CMDSYS_MVMT_ACCELLERATION;

	// zoom input, tilt & boundaries
	m_ZoomMomentum += g_Input.mouse.wheel*CMDSYS_ZOOM_ACCELLERATION;
	f32 __Bounds = glm::clamp(((g_Camera.distance+m_ZoomMomentum)-CMDSYS_ZOOM_MIDDIST)*CMDSYS_ZOOM_HALFDIST_INV,
							  -1.f,1.f);
	__Bounds = glm::pow(__Bounds,5.f);
	m_ZoomMomentum *= 1.f-abs(__Bounds)*((__Bounds<0&&m_ZoomMomentum<0)||(__Bounds>0&&m_ZoomMomentum>0));
	//g_Camera.pitch = CMDSYS_ZOOM_MINPITCH+(__Bounds+1.f)*CMDSYS_ZOOM_HALFPITCH;
	// TODO fix autopitch

	// update camera position
	g_Camera.target += m_CameraMomentum;
	g_Camera.distance += m_ZoomMomentum;
	g_Camera.update();

	// haptic attenuation
	m_CameraMomentum *= CMDSYS_MVMT_FLOATFACTOR;
	m_ZoomMomentum *= CMDSYS_ZOOM_FLOATFACTOR;
}
