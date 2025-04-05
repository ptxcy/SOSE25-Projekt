#include "input.h"


void Input::update(bool& running)
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT: running = false;
			break;
		};
	}
}
