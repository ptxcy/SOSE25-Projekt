#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/ui.h"


s32 main(s32 argc,char** argv)
{
	UIBatch uib;// = UIBatch(1);
	Button* btn = uib.add_button("not yet ready","./res/kid.png","./res/test.png","./res/maps.png",
								 vec2(100,100),vec2(100,100));
	g_UI.batches.push_back(&uib);
	Font __Ubuntu;
	g_Renderer.register_font(&__Ubuntu,"res/font/ubuntu.ttf",50);
	g_Renderer.write_text(&__Ubuntu,"es ist kein tier so klein dass es kein bruder koennte von dir sein",
						  vec2(0,0),25,vec4(1,1,1,1),SCREEN_ALIGN_CENTER);

	bool running = true;
	while (running)
	{
		g_Frame.clear();
		g_Input.update(running);

		if (btn->confirm) std::cout << "sync\n";
		g_UI.update();
		g_Renderer.update();
		g_Frame.update();
	}

	g_Renderer.exit();
	g_Frame.close();
	return 0;
}
