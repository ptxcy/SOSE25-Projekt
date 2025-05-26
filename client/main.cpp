#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"
#include "core/ui.h"
#include "core/http-adapter.h"
#include "core/websocket.h"


s32 main(s32 argc,char** argv)
{
	/*
	UIBatch uib;// = UIBatch(1);
	Button* btn = uib.add_button("not yet ready","./res/kid.png","./res/test.png","./res/maps.png",
								 vec2(100,100),vec2(100,100));
	g_UI.batches.push_back(&uib);
	*/

	Adapter ddd = Adapter("0.0.0.0","8080");
	string name = argv[1];
	string pass = argv[2];
	string lnom = argv[3];
	string macher = argv[4];
	COMM_ERR_COND(ddd.createUser(name,pass),"fuck din workk");
	std::optional<string> tokken = ddd.authenticateOnServer(name,pass);
	if (macher=="kool") ddd.createLobby(lnom,std::nullopt,*tokken);
	else ddd.joinLobby(lnom,std::nullopt,*tokken);
	COMM_LOG("%s",tokken.value().c_str());
	Websocket g_Websocket = Websocket(tokken.value(),"0.0.0.0","8083");

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

	bool running = true;
	while (running)
	{
		g_Frame.clear();
		g_Input.update(running);

		// send updates
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
