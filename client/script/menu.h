#ifndef SCRIPT_MENU_HEADER
#define SCRIPT_MENU_HEADER


#include "../core/wheel.h"
#include "../core/websocket.h"
#include "../core/ui.h"


struct Menu
{
	// utility
	void load(Font* font);
	static void update(u8* menu);
	void close();

	// data
	// correlation byte
	u8 id = 0x00;
	lptr<UpdateRoutine> ref;

	// textures
	PixelBufferComponent* button_hover;
	PixelBufferComponent* button_select;
	PixelBufferComponent* textbox_idle;
	PixelBufferComponent* textbox_hover;
	PixelBufferComponent* textbox_active;

	// ui components
	lptr<UIBatch> conn_batch;
	lptr<TextField> tfname;
	lptr<TextField> tfpass;
	lptr<TextField> tflobby;
	lptr<TextField> tflpass;
	lptr<Button> btjoin;
	lptr<Button> btcreate;
};


#endif
