#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"


s32 main(s32 argc,char** argv)
{
	Sprite* bigger = g_Renderer.register_sprite(vec2(400,200),vec2(200,200),70.f);
	Sprite* smaller = g_Renderer.register_sprite(vec2(100,100),vec2(100,100),-30.f);

	bool running = true;
	while(running)
	{
		g_Frame.clear();

		// sample usage
		smaller->rotation -= 2.f;
		smaller->rotation = fmod(smaller->rotation,360.f);
		bigger->rotation += 3.5f;
		bigger->rotation = fmod(bigger->rotation,360.f);

		Input::update(running);
		g_Renderer.update();

		g_Frame.update();
	}

	g_Frame.close();
	return 0;
}
