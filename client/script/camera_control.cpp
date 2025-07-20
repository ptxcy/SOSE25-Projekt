#include "camera_control.h"


/**
 *	setup origin camera projection
 */
CameraController::CameraController(Font* font)
{
	m_FPS = g_Renderer.write_text(font,"",vec3(-10,-10,0),15,vec4(1),Alignment{ .align=SCREEN_ALIGN_TOPRIGHT });
	g_Wheel.call(UpdateRoutine{ &CameraController::_update,(void*)this });
}

/**
 *	update camera math based on input
 */
void CameraController::update()
{
	// zoom input & boundaries
	m_ZoomMomentum += g_Input.mouse.wheel*CAMCNTR_ZOOM_ACCELLERATION;
	f32 __Pred = g_Camera.distance+m_ZoomMomentum;
	/*
	m_ZoomMomentum *= ((__Pred<CAMCNTR_ZOOM_MINDIST&&m_ZoomMomentum<.0f)
					   ||(__Pred>CAMCNTR_ZOOM_MAXDIST&&m_ZoomMomentum>.0f)) ? CAMCNTR_ZOOM_EASE : 1.f;
	*/

	// camera rotational orbit
	m_RotMomentum.x += (g_Input.keyboard.keys[SDL_SCANCODE_E]-g_Input.keyboard.keys[SDL_SCANCODE_Q])
			*CAMCNTR_ROT_KEYACC;
	m_RotMomentum += vec2(g_Input.mouse.buttons[1]*CAMCNTR_ROT_MOUSEACC)*g_Input.mouse.velocity;
	g_Camera.yaw += glm::radians(-m_RotMomentum.x);
	g_Camera.pitch += glm::radians(m_RotMomentum.y);

	// update camera position
	vec3 __Attitude = glm::normalize(vec3(g_Camera.target.x-g_Camera.position.x,
										  g_Camera.target.y-g_Camera.position.y,0));
	vec3 __OrthoAttitude = vec3(-__Attitude.y,__Attitude.x,0);
	m_CameraMomentum += (vec3(g_Input.keyboard.keys[SDL_SCANCODE_W]
							  - g_Input.keyboard.keys[SDL_SCANCODE_S])*__Attitude
						 + vec3(g_Input.keyboard.keys[SDL_SCANCODE_A]
							   - g_Input.keyboard.keys[SDL_SCANCODE_D])*__OrthoAttitude)
			* CAMCNTR_MVMT_ACCEL;
	g_Camera.target += m_CameraMomentum;
	g_Camera.distance += m_ZoomMomentum;
	m_CameraMomentum *= CAMCNTR_MVMT_FLOATFACTOR;

	// haptic attenuation
	m_ZoomMomentum *= CAMCNTR_ZOOM_FLOATFACTOR;
	m_RotMomentum *= CAMCNTR_ROT_FLOATFACTOR;

	// fps display
#ifdef DEBUG
	m_FPS->data = "FPS "+std::to_string(g_Frame.fps);
	m_FPS->align();
	m_FPS->load_buffer();
#endif
}
