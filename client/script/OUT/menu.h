#ifndef SCRIPT_MENU_HEADER
#define SCRIPT_MENU_HEADER


#include "../core/wheel.h"
#include "../core/websocket.h"
#include "../core/ui.h"

#include "command_center.h"


class Menu
{
public:
	Menu(Font* font,CommandCenter* cc);
	static inline void _update(void* menu) { Menu* p = (Menu*)menu; p->update(); }
	void update();
	void close();

private:

	// correlation
	lptr<UpdateRoutine> ref;
	CommandCenter* m_CC;

	// textures
	PixelBufferComponent* button_idle;
	PixelBufferComponent* button_hover;
	PixelBufferComponent* button_select;
	PixelBufferComponent* textbox_idle;
	PixelBufferComponent* textbox_hover;
	PixelBufferComponent* textbox_active;

	// names
	lptr<Text> __TUsr;
	lptr<Text> __TPsw;
	lptr<Text> __TLby;

	// ui components
	lptr<UIBatch> conn_batch;
	lptr<TextField> tfname;
	lptr<TextField> tfpass;
	lptr<TextField> tflobby;
	lptr<Button> btjoin;
	lptr<Button> btcreate;
};


#endif
