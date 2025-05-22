#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/ui.h"
#include "core/websocket.h"


s32 main(s32 argc,char** argv)
{
	UIBatch uib;// = UIBatch(1);
	Button* btn = uib.add_button("not yet ready","./res/kid.png","./res/test.png","./res/maps.png",
								 vec2(150,150),vec2(200,150));
	g_UI.batches.push_back(&uib);

	bool running = true;
	while (running)
	{
		g_Frame.clear();
		g_Input.update(running);

		if (btn->confirm)
		{
			g_Websocket.send_message({
					.request_info = {  },
					.request_data =  { .spawn_dummy = std::optional<std::string>("owen wilson") },
				});
		}

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
