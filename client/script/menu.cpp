#ifdef PROJECT_SPACER
#include "menu.h"


/**
 *	menu loader routine
 *	\param main menu font for lobby login
 */
Menu::Menu(Font* font,CommandCenter* cc)
	: m_CC(cc)
{
	// load textures
	button_idle = g_Renderer.register_sprite_texture("./res/ui/button_idle.png");
	button_hover = g_Renderer.register_sprite_texture("./res/ui/button_hover.png");
	button_select = g_Renderer.register_sprite_texture("./res/ui/button_on.png");
	textbox_idle = g_Renderer.register_sprite_texture("./res/ui/textbox.png");
	textbox_hover = g_Renderer.register_sprite_texture("./res/ui/textbox_hover.png");
	textbox_active = g_Renderer.register_sprite_texture("./res/ui/textbox_on.png");

	// constants
	vec2 tfsize = vec2(300,35);
	vec2 btsize = vec2(125,30);
	f32 tftitle = tfsize.y*.75f;
	f32 ttsize = btsize.y*.5f;
	f32 tfdist = 70.f;
	f32 tfstart = tfdist*.5f;
	f32 bteyez = tfsize.x*.475f-btsize.x*.5f;

	// setup lobby login ui
	conn_batch = g_UI.add_batch(font);
	tfname = conn_batch->add_text_field(textbox_idle,textbox_hover,textbox_active,vec3(0,tfstart+tfdist,0),
										tfsize,{.align=SCREEN_ALIGN_CENTER});
	tfpass = conn_batch->add_text_field(textbox_idle,textbox_hover,textbox_active,vec3(0,tfstart,0),
										tfsize,{.align=SCREEN_ALIGN_CENTER});
	tfpass->hidden = true;
	tflobby = conn_batch->add_text_field(textbox_idle, textbox_hover, textbox_active, vec3(0, -tfstart, 0),
										 tfsize, {.align = SCREEN_ALIGN_CENTER});

	btjoin = conn_batch->add_button("Join Lobby",button_idle,button_hover,button_select,
									vec3(-bteyez,-tfstart-tfdist,0),btsize,{.align=SCREEN_ALIGN_CENTER});
	btcreate = conn_batch->add_button("Create Lobby",button_idle,button_hover,button_select,
									  vec3(bteyez,-tfstart-tfdist,0),btsize,{.align=SCREEN_ALIGN_CENTER});

	// setup player communication
	__TUsr = g_Renderer.write_text(font,"username",tfname->canvas->offset+vec3(0,tftitle,0),ttsize);
	__TPsw = g_Renderer.write_text(font,"password",tfpass->canvas->offset+vec3(0,tftitle,0),ttsize);
	__TLby = g_Renderer.write_text(font,"lobby",tflobby->canvas->offset+vec3(0,tftitle,0),ttsize);

	// register routine
	ref = g_Wheel.call(UpdateRoutine{ &Menu::_update,(void*)this });
}

/**
 *	menu update routine
 */
void Menu::update()
{
	// react to confirmation
	if (btjoin->confirm||btcreate->confirm)
	{
		bool createLobby = btcreate->confirm;

#ifdef FEAT_MULTIPLAYER
		g_Websocket.connect(NETWORK_HOST,NETWORK_PORT_ADAPTER,NETWORK_PORT_WEBSOCKET,
							tfname->buffer,tfpass->buffer,tflobby->buffer,createLobby);
		if (g_Websocket.lobby_status!=LOBBY_CONNECTED) return;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		Request::connect();
		std::this_thread::sleep_for(std::chrono::milliseconds(NETWORK_CONNECTION_STALL));
		Request::set_fps(NETWORK_CALCULATION_FRAMES);
		while (!g_Websocket.state_update) { std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
#endif
		m_CC->run();
		close();
	}
}

/**
 *	cleanup menu
 */
void Menu::close()
{
	// deregister menu
	g_UI.remove_batch(conn_batch);

	// cleanup menu textures
	g_Renderer.delete_sprite_texture(button_idle);
	g_Renderer.delete_sprite_texture(button_hover);
	g_Renderer.delete_sprite_texture(button_select);
	g_Renderer.delete_sprite_texture(textbox_idle);
	g_Renderer.delete_sprite_texture(textbox_hover);
	g_Renderer.delete_sprite_texture(textbox_active);

	// remove title text
	g_Renderer.delete_text(__TUsr);
	g_Renderer.delete_text(__TPsw);
	g_Renderer.delete_text(__TLby);

	// kill menu update routine
	g_Wheel.routines.erase(ref);
}


#endif
