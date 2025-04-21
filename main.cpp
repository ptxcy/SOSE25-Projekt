#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"


std::vector<std::string> textures = { "./res/kid.png","./res/kek.png","./res/test.png","./res/maps.png" };

s32 main(s32 argc,char** argv)
{
	/*
	Sprite* bigger = g_Renderer.register_sprite(vec2(400,200),vec2(200,200),70.f);
	Sprite* smaller = g_Renderer.register_sprite(vec2(100,100),vec2(100,100),-30.f);
	Sprite* intermediate = nullptr;
	bool loaded_new = false;
	bool deleted_old = false;
	*/

	/*
	PixelBufferComponent* t0 = g_Renderer.register_sprite_texture("./res/kid.png");
	PixelBufferComponent* t1 = g_Renderer.register_sprite_texture("./res/kek.png");
	PixelBufferComponent* t2 = g_Renderer.register_sprite_texture("./res/test.png");
	PixelBufferComponent* t3 = g_Renderer.register_sprite_texture("./res/maps.png");
	*/
	u8 head = 0;
	u8 atex = 0;
	vec2 position = vec2(100,100);
	PixelBufferComponent* ts[4];
	//Sprite* atlas_preview = g_Renderer.register_sprite(t0,vec2(640,360),vec2(620,620));

	bool running = true;
	while(running)
	{
		g_Frame.clear();
		g_Input.update(running);

		if (g_Input.keyboard.triggered_keys[SDL_SCANCODE_L]&&head<4)
		{
			ts[head] = g_Renderer.register_sprite_texture(textures[head].c_str());
			head++;
		}
		if (g_Input.keyboard.triggered_keys[SDL_SCANCODE_O])
		{
			g_Renderer.register_sprite(ts[atex],position,vec2(100,100));
			position.x += 150;
			bool rpos = position.x>1150;
			position.y += 150*rpos;
			position.x = rpos ? 100 : position.x;
			atex = (atex+1)%head;
		}

		// sample usage
		/*
		if (!loaded_new&&g_Input.keyboard.keys[SDL_SCANCODE_L])
		{
			intermediate = g_Renderer.register_sprite(vec2(500,500),vec2(150,150),20.f);
			loaded_new = true;
		}
		if (!deleted_old&&g_Input.keyboard.keys[SDL_SCANCODE_O])
		{
			Renderer::delete_sprite(smaller);
			deleted_old = true;
		}

		// sample cased update
		if (loaded_new) intermediate->rotation
							+= (g_Input.keyboard.keys[SDL_SCANCODE_D]-g_Input.keyboard.keys[SDL_SCANCODE_A])*4.f;
		if (!deleted_old)
		{
			smaller->rotation -= 2.f;
			smaller->rotation = fmod(smaller->rotation,360.f);
		}
		bigger->rotation += 3.5f;
		bigger->rotation = fmod(bigger->rotation,360.f);
		*/

		g_Renderer.update();
		g_Frame.update();
	}

	g_Renderer.exit();
	g_Frame.close();
	return 0;
}
