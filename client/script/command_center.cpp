#include "command_center.h"


/**
 *	command center setup
 */
CommandCenter::CommandCenter(Font* font,StarSystem* ssys,Flotilla* flt)
	: m_Font(font),m_StarSystem(ssys),m_Flotilla(flt)
{
	g_Camera.distance = 2.f;
	g_Camera.target = m_StarSystem->planets[m_PlanetLock].offset;
	g_Camera.pitch = glm::radians(30.0f);
}

/**
 *	TODO placeholder until memory management fixed and this is finally on heap
 */
void CommandCenter::run()
{
	// create mode communication
	m_TxControlMode = g_Renderer.write_text(m_Font,"",vec2(15,-15),15,
											vec4(0),Alignment{ .align=SCREEN_ALIGN_TOPLEFT });
	_set_text_locked();

	// jumper menu
	m_ButtonIdle = g_Renderer.register_sprite_texture("./res/ui/button_idle.png");
	m_ButtonHover = g_Renderer.register_sprite_texture("./res/ui/button_hover.png");
	m_ButtonSelect = g_Renderer.register_sprite_texture("./res/ui/button_on.png");
	lptr<UIBatch> __UIBatch = g_UI.add_batch(m_Font,.75f);
	for (u8 i=0;i<8;i++) m_BtnJumpers[i] = __UIBatch->add_button(("jump to "+m_PlanetNames[i]).c_str(),
																 m_ButtonIdle,m_ButtonHover,m_ButtonSelect,
																 vec2(-15,(i-4)*-40),vec2(200,25),
																 Alignment{ .align=SCREEN_ALIGN_CENTERRIGHT });
	for (u8 i=0;i<10;i++) m_BtnFleet[i] = __UIBatch->add_button("Free Ship Slot",
																m_ButtonIdle,m_ButtonHover,m_ButtonSelect,
																vec2(15,(i-5)*-40),vec2(200,25),
																Alignment{ .align=SCREEN_ALIGN_CENTERLEFT });
	m_BtnBuild = __UIBatch->add_button("create spaceship",m_ButtonIdle,m_ButtonHover,m_ButtonSelect,
									   vec2(15,15),vec2(200,40),Alignment{ .align=SCREEN_ALIGN_BOTTOMLEFT });
	// FIXME multiple loads of the same button graphics

	g_Wheel.call(UpdateRoutine{ &CommandCenter::_update,(void*)this });
}

/**
 *	command center handling
 */
void CommandCenter::update()
{
	// jumper navigation
	for (u8 i=0;i<8;i++)
	{
		if (!m_BtnJumpers[i]->confirm) continue;

		if (m_CState==CSTATE_FLIGHT)
		{
#ifdef FEAT_MULTIPLAYER
			Request::set_spaceship_target(m_Flotilla->fleet[i].id,i);
#endif
		}
		else
		{
			m_PlanetLock = i;
			m_CState = CSTATE_LOCKED;
			_set_text_locked();
			g_Camera.distance = m_StarSystem->planets[i].scale*2.5f;
			g_Camera.pitch = glm::radians(10.f);
		}
	}

	// spawning spaceships
#ifdef FEAT_MULTIPLAYER
	if (m_BtnBuild->confirm) Request::spawn_spaceship(g_Camera.target+vec3(10,10,0));
#endif

	// button label update
	for (u8 i=0;i<10;i++)
	{
		if (i<m_Flotilla->fleet.size())
		{
			m_BtnFleet[i]->label->data = "Ship "+std::to_string(m_Flotilla->fleet[i].id);
			if (m_BtnFleet[i]->confirm)
			{
				m_ShipLock = i;
				m_CState = CSTATE_FLIGHT;
				_set_planet_buttons("fly to ");
				_set_text_flight();
			}
		}
		else m_BtnFleet[i]->label->data = "Free Ship Slot";
		m_BtnFleet[i]->label->align();
		m_BtnFleet[i]->label->load_buffer();
	}

	// control mode
	Spaceship __Spaceship;
	vec3 __Attitude,__OrthoAttitude;
	switch (m_CState)
	{
	case CSTATE_LOCKED:

		// switch to freeform movement mode
		if (g_Input.keyboard.triggered_keys[SDL_SCANCODE_TAB])
		{
			m_CState = CSTATE_FREEFORM;
			_set_text_freeform();
		}

		// locking camera target to orbiting planet
		g_Camera.target = m_StarSystem->planets[m_PlanetLock].offset;

		break;
	case CSTATE_FLIGHT:

		// spaceflight lock-on
		__Spaceship = m_Flotilla->fleet[m_ShipLock];
		g_Camera.target = vec3(m_Flotilla->fleet[m_ShipLock].position.x,
							   m_Flotilla->fleet[m_ShipLock].position.y,
							   m_Flotilla->fleet[m_ShipLock].position.z);

		// switch to freeform movement mode
		if (g_Input.keyboard.triggered_keys[SDL_SCANCODE_TAB])
		{
			m_CState = CSTATE_LOCKED;
			_set_planet_buttons("jump to ");
			_set_text_locked();
		}

		break;
	case CSTATE_FREEFORM:

		// switch to locked movement mode
		if (g_Input.keyboard.triggered_keys[SDL_SCANCODE_TAB])
		{
			m_CState = CSTATE_LOCKED;
			_set_text_locked();
		}

		// update camera target by input
		__Attitude = glm::normalize(vec3(g_Camera.target.x-g_Camera.position.x,
										 g_Camera.target.y-g_Camera.position.y,0));
		__OrthoAttitude = vec3(-__Attitude.y,__Attitude.x,0);
		m_CameraMomentum += (vec3(g_Input.keyboard.keys[SDL_SCANCODE_W]-g_Input.keyboard.keys[SDL_SCANCODE_S])
							 *__Attitude
							 +vec3(g_Input.keyboard.keys[SDL_SCANCODE_D]-g_Input.keyboard.keys[SDL_SCANCODE_A])
							 *__OrthoAttitude)
			*CMDSYS_MVMT_ACCELLERATION;
		g_Camera.target += m_CameraMomentum;

		break;
	};

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
	g_Camera.update();

	// haptic attenuation
	m_CameraMomentum *= CMDSYS_MVMT_FLOATFACTOR;
	m_ZoomMomentum *= CMDSYS_ZOOM_FLOATFACTOR;
	m_RotMomentum *= CMDSYS_ROT_FLOATFACTOR;
}

/**
 *	TODO
 */
void CommandCenter::_set_planet_buttons(string instr)
{
	for (u8 i=0;i<8;i++)
	{
		m_BtnJumpers[i]->label->data = instr+m_PlanetNames[i];
		m_BtnJumpers[i]->label->align();
		m_BtnJumpers[i]->label->load_buffer();
	}
}

/**
 *	TODO
 */
void CommandCenter::_set_text_locked()
{
	m_TxControlMode->data = "Orbiting "+m_PlanetNames[m_PlanetLock];
	m_TxControlMode->colour = vec4(.5f,0,0,1);
	m_TxControlMode->align();
	m_TxControlMode->load_buffer();
}

/**
 *	TODO
 */
void CommandCenter::_set_text_flight()
{
	m_TxControlMode->data = "Flying Spaceship "+std::to_string(m_Flotilla->fleet[m_ShipLock].id)
															   +" -> [TAB] to go back to "
															   +m_PlanetNames[m_PlanetLock];
	m_TxControlMode->colour = vec4(0,.5f,0,1);
	m_TxControlMode->align();
	m_TxControlMode->load_buffer();
}

/**
 *	TODO
 */
void CommandCenter::_set_text_freeform()
{
	m_TxControlMode->data = "System Exploration Mode -> [TAB] to jump to "+m_PlanetNames[m_PlanetLock];
	m_TxControlMode->colour = vec4(0,0,.5f,1);
	m_TxControlMode->align();
	m_TxControlMode->load_buffer();
}
