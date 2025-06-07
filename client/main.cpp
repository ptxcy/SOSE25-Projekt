#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/ui.h"
#include "core/wheel.h"
#include "core/websocket.h"

#include "script/menu.h"
#include "script/starsystem.h"


s32 main(s32 argc,char** argv)
{
	Font* __Ubuntu = g_Renderer.register_font("./res/font/ubuntu.ttf",30);
	CommandCenter __CCenter = CommandCenter();
	Menu __Menu = Menu(__Ubuntu,&__CCenter);
	StarSystem __StarSystem = StarSystem();

	bool running = true;
	while (running)
	{
		g_Frame.clear();
		g_Input.update(running);
		g_Wheel.update();
		g_UI.update();
		g_Renderer.update();
		g_Frame.update();
	}

#ifdef FEAT_MULTIPLAYER
	g_Websocket.exit();
#endif
	g_Renderer.exit();
	g_Frame.close();
	return 0;
}
