#ifndef SCRIPT_COMMANDCENTER_HEADER
#define SCRIPT_COMMANDCENTER_HEADER


#include "../core/input.h"
#include "../core/ui.h"
#include "../core/wheel.h"
#include "webcomm.h"
#include "starsystem.h"
#include "flotilla.h"


// movement
constexpr f32 CMDSYS_MVMT_ACCELLERATION = .15f;
constexpr f32 CMDSYS_MVMT_FLOATFACTOR = .8f;


enum ControlState
{
	CSTATE_LOCKED,
	CSTATE_FLIGHT,
	CSTATE_FREEFORM
};


class CommandCenter
{
public:
	CommandCenter(Font* font,StarSystem* ssys,Flotilla* flt);
	void run();
	static inline void _update(void* cc) { CommandCenter* p = (CommandCenter*)cc; p->update(); }
	void update();
	// TODO better routine persistence through wheelcall also handling struct memory

private:

	// text updates
	void _set_planet_buttons(string instr);
	void _set_text_locked();
	void _set_text_flight();
	void _set_text_freeform();

private:

	// components
	Font* m_Font;
	StarSystem* m_StarSystem;
	Flotilla* m_Flotilla;

	// camera movement
	vec3 m_CameraMomentum = vec3(0);
	ControlState m_CState = CSTATE_LOCKED;
	u8 m_PlanetLock = 2;
	u8 m_ShipLock = 0;
	u64 m_ShipChosen = 0;

	// ui
	PixelBufferComponent* m_ButtonIdle;
	PixelBufferComponent* m_ButtonHover;
	PixelBufferComponent* m_ButtonSelect;
	lptr<Text> m_TxControlMode;
	lptr<Button> m_BtnJumpers[8];
	lptr<Button> m_BtnFleet[10];
	lptr<Button> m_BtnBuild;

	// data
	string m_PlanetNames[8] = { "Mercury","Venus","Earth","Mars","Jupiter","Saturn","Uranus","Neptune" };
};


#endif
