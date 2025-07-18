#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/ui.h"
#include "core/wheel.h"
#include "core/websocket.h"

#include "script/camera_control.h"
/*
#include "script/menu.h"
#include "script/starsystem.h"
#include "script/server_update.h"
#include "script/planet_focus.h"
//#include "script/test.h"
*/
#include "script/pong.h"


s32 main(s32 argc,char** argv)
{
	Font* __Ubuntu = g_Renderer.register_font("./res/font/ubuntu.ttf",20);
	/*
	StarSystem __StarSystem = StarSystem();
	Flotilla __Flotilla = Flotilla();
	CommandCenter __CCenter = CommandCenter(__Ubuntu,&__StarSystem,&__Flotilla);
#ifdef FEAT_MULTIPLAYER
	ServerUpdate __ServerUpdate = ServerUpdate(&__StarSystem,&__Flotilla);
#endif
	Menu __Menu = Menu(__Ubuntu,&__CCenter);
	*/

	CameraController __CamControl = CameraController(__Ubuntu);
	Pong __Pong = Pong(__Ubuntu,argv[1]);
	//TestScene __TestScene = TestScene();
	//PlanetFocus __PlanetFocus = PlanetFocus(__Ubuntu);

	bool running = true;
	while (running)
	{
		g_Frame.clear();
		g_Input.update(running);
		g_Wheel.update();
		g_Camera.update();
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
