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
	m_CameraMomentum.x += (g_Input.keyboard.keys[SDL_SCANCODE_A]-g_Input.keyboard.keys[SDL_SCANCODE_D])
			* CMDSYS_MVMT_ACCELLERATION;
	m_CameraMomentum.z += (g_Input.keyboard.keys[SDL_SCANCODE_W]-g_Input.keyboard.keys[SDL_SCANCODE_S])
			* CMDSYS_MVMT_ACCELLERATION;

	// zoom input & boundaries
	m_CameraMomentum.y += g_Input.mouse.wheel*CMDSYS_ZOOM_ACCELLERATION;
	// TODO boundaries (that's what she said)

	// interpolate camera towards target position
	g_Camera.position += m_CameraMomentum;
	g_Camera.target += vec3(m_CameraMomentum.x,0,m_CameraMomentum.z);
	g_Camera.update();

	// haptic attenuation
	m_CameraMomentum *= CMDSYS_MOMENTUM_FLOATFACTOR;
}
