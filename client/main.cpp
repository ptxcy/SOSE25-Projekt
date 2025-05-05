#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/ui.h"


std::vector<std::string> textures = { "./res/kid.png","./res/kek.png","./res/test.png","./res/maps.png" };

s32 main(s32 argc,char** argv)
{
	/*
	u8 head = 0;
	u8 atex = 0;
	u8 timer = 0;
	PixelBufferComponent* ts[4];
	ts[head] = g_Renderer.register_sprite_texture(textures[head].c_str());
	Sprite* s0 = g_Renderer.register_sprite(ts[head],vec2(100,100),vec2(150,150));
	Sprite* s1 = g_Renderer.register_sprite(ts[head],vec2(400,120),vec2(100,100));
	Sprite* s2 = g_Renderer.register_sprite(ts[head],vec2(120,400),vec2(200,200));
	Sprite* s3 = g_Renderer.register_sprite(ts[head],vec2(400,400),vec2(50,50));
	head++;
	*/
	//UIBatch* uib = g_UI.batches.next_free();
	UIBatch uib;
	Button* btn = uib.add_button("not yet ready","./res/kid.png","./res/test.png","./res/maps.png",
								 vec2(150,150),vec2(200,150));
	g_UI.batches.push_back(&uib);

	bool running = true;
	while(running)
	{
		g_Frame.clear();
		g_Input.update(running);

		/*
		if (g_Input.keyboard.triggered_keys[SDL_SCANCODE_L]&&head<4)
		{
			ts[head] = g_Renderer.register_sprite_texture(textures[head].c_str());
			head++;
		}
		if (g_Input.keyboard.triggered_keys[SDL_SCANCODE_O]) Renderer::delete_sprite(s3);
		if (g_Input.keyboard.triggered_keys[SDL_SCANCODE_P]) Renderer::delete_sprite(s1);
		if (g_Input.keyboard.triggered_keys[SDL_SCANCODE_T]) Renderer::delete_sprite_texture(ts[head-1]),head--;

		if (timer>20)
		{
			atex = (atex+1)%head;
			g_Renderer.assign_sprite_texture(s0,ts[atex]);
			g_Renderer.assign_sprite_texture(s1,ts[atex]);
			g_Renderer.assign_sprite_texture(s2,ts[atex]);
			g_Renderer.assign_sprite_texture(s3,ts[atex]);
			timer = 0;
		}
		timer++;
		*/

		if (btn->confirm) std::cout << "sync\n";
		g_UI.update();
		g_Renderer.update();
		g_Frame.update();
	}

	g_Renderer.exit();
	g_Frame.close();
	return 0;
}
