#ifndef SYSTEM_UI_HEADER
#define SYSTEM_UI_HEADER


#include "input.h"
#include "renderer.h"


struct Button
{
	Sprite* canvas;
	PixelBufferComponent* idle;
	PixelBufferComponent* hover;
	PixelBufferComponent* action;
	lptr<Text> label;
	Rect bounds;
	bool holding = false;
	bool confirm = false;
};

struct TextField
{
	Sprite* canvas;
	PixelBufferComponent* idle;
	PixelBufferComponent* hover;
	PixelBufferComponent* action;
	lptr<Text> content;
	Rect bounds;
	bool active = false;
};
// TODO load textures for idle, hover and action once for each entity to save massive VRAM

struct UIBatch
{
	// utility
	lptr<Button> add_button(const char* label,string tidle,string thover,string taction,vec2 position,
							vec2 scale,Alignment alignment={});
	lptr<TextField> add_text_field(string tidle,string thover,string tselect,vec2 position,
								   vec2 scale,Alignment alignment={});

	// data
	Font* font;
	list<Button> buttons;
	list<TextField> tfields;
};


class UI
{
public:
	void update();

	// registration
	lptr<UIBatch> add_batch(Font* font);

private:
	Font* m_Font;
	list<UIBatch> m_Batches;
};

inline UI g_UI = UI();


#endif
