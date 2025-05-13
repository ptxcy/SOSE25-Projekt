#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/ui.h"


s32 main(s32 argc,char** argv)
{
	UIBatch uib;// = UIBatch(1);
	Button* btn = uib.add_button("not yet ready","./res/kid.png","./res/test.png","./res/maps.png",
								 vec2(640,360),vec2(720,720));
	g_UI.batches.push_back(&uib);
	//Font __Ubuntu;
	//g_Renderer.register_font(&__Ubuntu,"res/font/ubuntu.ttf",50);

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
