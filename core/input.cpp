#include "input.h"


/**
 *	update peripheral state must be called in the main loop, to be able to read user input correctly
 *	\param running: reference to program running state, closing requests will force this to false
 */
void Input::update(bool& running)
{
	mouse.wheel = 0;

	// reset trigger input per frame
	keyboard.triggered_keys.reset();
	mouse.triggered_buttons.reset();
	// FIXME trigger logic utterly broken

	// process peripheral events
	while (SDL_PollEvent(&m_Event))
	{
		switch (m_Event.type)
		{

			// keyboard input
		case SDL_KEYDOWN:
			keyboard.keys.set(m_Event.key.keysym.scancode);
			keyboard.triggered_keys.set(m_Event.key.keysym.scancode);
			break;
		case SDL_KEYUP: keyboard.keys.unset(m_Event.key.keysym.scancode);
			break;

			// mouse input
		case SDL_MOUSEMOTION:
			SDL_GetMouseState((int*)&mouse.position.x,(int*)&mouse.position.y);
			mouse.position *= vec2(FRAME_RESOLUTION_X_INV,FRAME_RESOLUTION_Y_INV);
			mouse.position *= vec2(MATH_CARTESIAN_XRANGE,MATH_CARTESIAN_YRANGE);
			break;
		case SDL_MOUSEBUTTONDOWN:
			mouse.buttons.set(m_Event.button.button-1);
			mouse.triggered_buttons.set(m_Event.button.button-1);
			break;
		case SDL_MOUSEBUTTONUP: mouse.buttons.unset(m_Event.button.button-1);
			break;
		case SDL_MOUSEWHEEL: mouse.wheel = m_Event.wheel.y;
			break;

			// system closing request
		case SDL_QUIT: running = false;
			break;
		};
	}
}
