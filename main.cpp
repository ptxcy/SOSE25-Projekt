#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"


s32 main(s32 argc,char** argv)
{
	g_Renderer.sprites.push_back({ .offset = vec2(100,100),.scale = vec2(100,100),.rotation = -30.f });
	g_Renderer.sprites.push_back({ .offset = vec2(400,200),.scale = vec2(200,200),.rotation = 70.f });
	g_Renderer.load_sprites();

	bool running = true;
	while(running)
	{
		g_Frame.clear();

		// sample usage
		g_Renderer.sprites[0].rotation += -2.f;
		g_Renderer.sprites[0].rotation -= 360.f*(g_Renderer.sprites[0].rotation>360.f);
		g_Renderer.sprites[1].rotation += 3.5f;
		g_Renderer.sprites[1].rotation += 360.f*(g_Renderer.sprites[1].rotation<-360.f);
		g_Renderer.load_sprites();

		Input::update(running);
		g_Renderer.update();

		g_Frame.update();
	}

	g_Frame.close();
	return 0;
}
