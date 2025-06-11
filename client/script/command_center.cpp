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
	g_Camera.pitch = glm::radians(30.0f);
}

/**
 *	command center handling
 */
void CommandCenter::update()
{
	// camera movement
	vec3 __Attitude = glm::normalize(vec3(g_Camera.target.x-g_Camera.position.x,
										  g_Camera.target.y-g_Camera.position.y,0));
	vec3 __OrthoAttitude = vec3(-__Attitude.y,__Attitude.x,0);
	m_CameraMomentum += (vec3(g_Input.keyboard.keys[SDL_SCANCODE_W]-g_Input.keyboard.keys[SDL_SCANCODE_S])
						 *__Attitude
						 +vec3(g_Input.keyboard.keys[SDL_SCANCODE_D]-g_Input.keyboard.keys[SDL_SCANCODE_A])
						 *__OrthoAttitude)
			*CMDSYS_MVMT_ACCELLERATION;

	// zoom input & boundaries
	m_ZoomMomentum += g_Input.mouse.wheel*CMDSYS_ZOOM_ACCELLERATION;
	f32 __Pred = g_Camera.distance+m_ZoomMomentum;
	m_ZoomMomentum *= ((__Pred<CMDSYS_ZOOM_MINDIST&&m_ZoomMomentum<.0f)
					   ||(__Pred>CMDSYS_ZOOM_MAXDIST&&m_ZoomMomentum>.0f)) ? CMDSYS_ZOOM_EASE : 1.f;

	// camera rotational orbit
	m_RotMomentum.x += (g_Input.keyboard.keys[SDL_SCANCODE_E]-g_Input.keyboard.keys[SDL_SCANCODE_Q])
			*CMDSYS_ROT_KEYACC;
	m_RotMomentum += vec2(g_Input.mouse.buttons[1]*CMDSYS_ROT_MOUSEACC)*g_Input.mouse.velocity;
	g_Camera.yaw += glm::radians(m_RotMomentum.x);
	g_Camera.pitch += glm::radians(m_RotMomentum.y);

	// update camera position
	g_Camera.target += m_CameraMomentum;
	g_Camera.distance += m_ZoomMomentum;
	g_Camera.update();

	// haptic attenuation
	m_CameraMomentum *= CMDSYS_MVMT_FLOATFACTOR;
	m_ZoomMomentum *= CMDSYS_ZOOM_FLOATFACTOR;
	m_RotMomentum *= CMDSYS_ROT_FLOATFACTOR;
}
