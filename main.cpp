#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"


s32 main(s32 argc,char** argv)
{
	Sprite* bigger = g_Renderer.register_sprite(vec2(400,200),vec2(200,200),70.f);
	Sprite* smaller = g_Renderer.register_sprite(vec2(100,100),vec2(100,100),-30.f);
	Sprite* intermediate = nullptr;
	bool loaded_new = false;

	bool running = true;
	while(running)
	{
		g_Frame.clear();
		g_Input.update(running);

		// sample usage
		if (!loaded_new&&g_Input.keyboard.keys[SDL_SCANCODE_L])
		{
			intermediate = g_Renderer.register_sprite(vec2(500,500),vec2(150,150),20.f);
			loaded_new = true;
		}
		if (loaded_new) intermediate->rotation
							+= (g_Input.keyboard.keys[SDL_SCANCODE_D]-g_Input.keyboard.keys[SDL_SCANCODE_A])*4.f;
		smaller->rotation -= 2.f;
		smaller->rotation = fmod(smaller->rotation,360.f);
		bigger->rotation += 3.5f;
		bigger->rotation = fmod(bigger->rotation,360.f);

		g_Renderer.update();
		g_Frame.update();
	}

	g_Frame.close();
	return 0;
}
