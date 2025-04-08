#include "input.h"


/**
 *	update peripheral state must be called in the main loop, to be able to read user input correctly
 *	\param running: reference to program running state, closing requests will force this to false
 */
void Input::update(bool& running)
{
	mouse.wheel = 0;

	// process peripheral events
	while (SDL_PollEvent(&m_Event))
	{
		switch (m_Event.type)
		{

			// keyboard input
		case SDL_KEYDOWN: keyboard.keys[m_Event.key.keysym.scancode] = true;
			break;
		case SDL_KEYUP: keyboard.keys[m_Event.key.keysym.scancode] = false;
			break;

			// mouse input
		case SDL_MOUSEMOTION:
			SDL_GetMouseState((int*)&mouse.position.x,(int*)&mouse.position.y);
			mouse.position *= vec2(FRAME_RESOLUTION_X_INV,FRAME_RESOLUTION_Y_INV);
			mouse.position *= vec2(MATH_CARTESIAN_XRANGE,MATH_CARTESIAN_YRANGE);
			break;
		case SDL_MOUSEBUTTONDOWN: mouse.buttons[m_Event.button.button-1] = true;
			break;
		case SDL_MOUSEBUTTONUP: mouse.buttons[m_Event.button.button-1] = false;
			break;
		case SDL_MOUSEWHEEL: mouse.wheel = m_Event.wheel.y;
			break;

			// system closing request
		case SDL_QUIT: running = false;
			break;
		};
	}
}
