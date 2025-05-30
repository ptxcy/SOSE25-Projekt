#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/ui.h"
#include "core/websocket.h"


s32 main(s32 argc,char** argv)
{
	Font* __Ubuntu = g_Renderer.register_font("./res/font/ubuntu.ttf",50);
	PixelBufferComponent* __ButtonHover = g_Renderer.register_sprite_texture("./res/ui/button_hover.png");
	PixelBufferComponent* __ButtonSelect = g_Renderer.register_sprite_texture("./res/ui/button_on.png");
	lptr<UIBatch> uib = g_UI.add_batch(__Ubuntu);
	lptr<Button> btn0 = uib->add_button("destroy",__ButtonHover,__ButtonSelect,__ButtonHover,
										vec2(100,100),vec2(150,40));
	lptr<Button> btn1 = uib->add_button("erase",__ButtonHover,__ButtonSelect,__ButtonHover,
										vec2(0,0),vec2(150,40),{ .align=SCREEN_ALIGN_TOPCENTER });
	lptr<Button> btn2 = uib->add_button("improve",__ButtonHover,__ButtonSelect,__ButtonHover,
										vec2(0,0),vec2(150,40),{ .align=SCREEN_ALIGN_BOTTOMRIGHT });
	lptr<TextField> tf0 = uib->add_text_field(__ButtonHover,__ButtonSelect,__ButtonSelect,vec2(0,65),
											  vec2(700,50),{ .align=SCREEN_ALIGN_CENTER });
	lptr<TextField> tf1 = uib->add_text_field(__ButtonHover,__ButtonSelect,__ButtonSelect,vec2(0,0),
											  vec2(700,50),{ .align=SCREEN_ALIGN_CENTER });
	lptr<TextField> tf2 = uib->add_text_field(__ButtonHover,__ButtonSelect,__ButtonSelect,vec2(0,-65),
											  vec2(700,50),{ .align=SCREEN_ALIGN_CENTER });

	/*
	string name = argv[1];
	std::map<string,Sprite*> entities;
	PixelBufferComponent* t0 = g_Renderer.register_sprite_texture("./res/kid.png");
	entities[name] = g_Renderer.register_sprite(t0,vec2(150),vec2(100));
	g_Websocket.send_message({
			.request_info = {  },
			.request_data = { .spawn_dummy = std::optional<string>(name) },
		});

	g_Websocket.send_message({
			.request_info = {  },
			.request_data = {
				.set_client_fps = std::optional<SetClientFPS>({
					.id = name,
					.fps = 60.
				})
			}
		});
	*/

	bool running = true;
	bool logic = true;
	while (running)
	{
		g_Frame.clear();
		g_Input.update(running);

		if (logic&&btn0->confirm)
		{
			g_UI.remove_batch(uib);
			logic = false;
		}

		// send updates
		/*
		g_Websocket.send_message({
				.request_info = {},
				.request_data = {
					.dummy_set_velocity = std::optional<DummySetVelocity>({
						.id = name,
						.position = {
							.x = 100*(g_Input.keyboard.keys[SDL_SCANCODE_D]-g_Input.keyboard.keys[SDL_SCANCODE_A]),
							.y = 100*(g_Input.keyboard.keys[SDL_SCANCODE_W]-g_Input.keyboard.keys[SDL_SCANCODE_S]),
							.z = .0
						}
					})
				},
			});

		// handle messages
		while (g_Websocket.server_messages.size())
		{
			ServerMessage p_Msg = g_Websocket.receive_message();
			for (const auto& msg : p_Msg.request_data.game_objects.dummies)
			{
				if (entities.find(msg.first)==entities.end())
				{
					entities[msg.first] = g_Renderer.register_sprite(t0,vec2(150),vec2(100));
					// FIXME slow as shit kill me please
				}
				entities[msg.first]->offset = vec3(msg.second.position.x,msg.second.position.y,msg.second.position.z);
			}
		}
		*/

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
