#include "menu.h"


/**
 *	menu loader routine
 *	\param main menu font for lobby login
 */
Menu::Menu(Font* font)
{
	// load textures
	button_hover = g_Renderer.register_sprite_texture("./res/ui/button_hover.png");
	button_select = g_Renderer.register_sprite_texture("./res/ui/button_on.png");
	textbox_idle = g_Renderer.register_sprite_texture("./res/ui/textbox.png");
	textbox_hover = g_Renderer.register_sprite_texture("./res/ui/textbox_hover.png");
	textbox_active = g_Renderer.register_sprite_texture("./res/ui/textbox_on.png");

	// setup lobby login ui
	conn_batch = g_UI.add_batch(font);
	tfname = conn_batch->add_text_field(textbox_idle,textbox_hover,textbox_active,vec2(0,90),vec2(500,50),
										   { .align=SCREEN_ALIGN_CENTER });
	tfpass = conn_batch->add_text_field(textbox_idle,textbox_hover,textbox_active,vec2(0,30),vec2(500,50),
										   { .align=SCREEN_ALIGN_CENTER });
	tflobby = conn_batch->add_text_field(textbox_idle,textbox_hover,textbox_active,vec2(0,-30),vec2(500,50),
											{ .align=SCREEN_ALIGN_CENTER });
	tflpass = conn_batch->add_text_field(textbox_idle,textbox_hover,textbox_active,vec2(0,-90),vec2(500,50),
											{ .align=SCREEN_ALIGN_CENTER });
	btjoin = conn_batch->add_button("Join Lobby",button_hover,button_select,button_hover,
									vec2(-125,-145),vec2(175,40),{ .align=SCREEN_ALIGN_CENTER });
	btcreate = conn_batch->add_button("Create Lobby",button_hover,button_select,button_hover,
									  vec2(125,-145),vec2(175,40),{ .align=SCREEN_ALIGN_CENTER });

	// register routine
	ref = g_Wheel.call(UpdateRoutine{ &Menu::update,(u8*)this });
}

/**
 *	menu update routine
 */
void Menu::_update()
{
	// react to confirmation
	if (btjoin->confirm||btcreate->confirm)
	{
#ifdef FEAT_MULTIPLAYER
		g_Websocket.connect(NETWORK_HOST,NETWORK_PORT_ADAPTER,NETWORK_PORT_WEBSOCKET,
							tfname->get_content(),tfpass->get_content(),tflobby->get_content(),
							tflpass->get_content(),btcreate->confirm);
#endif
		close();
	}
}

/**
 *	cleanup menu
 */
void Menu::close()
{
	g_UI.remove_batch(conn_batch);
	g_Renderer.delete_sprite_texture(button_hover);
	g_Renderer.delete_sprite_texture(button_select);
	g_Renderer.delete_sprite_texture(textbox_idle);
	g_Renderer.delete_sprite_texture(textbox_hover);
	g_Renderer.delete_sprite_texture(textbox_active);
	g_Wheel.routines.erase(ref);
}
