#include "camera_control.h"


/**
 *	setup origin camera projection
 */
CameraController::CameraController()
{
	g_Wheel.call(UpdateRoutine{ &CameraController::_update,(void*)this });
}

/**
 *	update camera math based on input
 */
void CameraController::update()
{
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
	g_Camera.distance += m_ZoomMomentum;

	// haptic attenuation
	m_ZoomMomentum *= CMDSYS_ZOOM_FLOATFACTOR;
	m_RotMomentum *= CMDSYS_ROT_FLOATFACTOR;
}
