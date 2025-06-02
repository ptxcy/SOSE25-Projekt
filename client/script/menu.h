#ifndef SCRIPT_MENU_HEADER
#define SCRIPT_MENU_HEADER


#include "../core/wheel.h"
#include "../core/websocket.h"
#include "../core/ui.h"


class Menu
{
public:
	Menu(Font* font);
	static inline void update(u8* menu) { Menu* p = (Menu*)menu; p->_update(); }
	void _update();
	void close();

private:
	// correlation
	lptr<UpdateRoutine> ref;
	bool run = true;

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
