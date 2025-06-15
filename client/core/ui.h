#ifndef SYSTEM_UI_HEADER
#define SYSTEM_UI_HEADER


#include "input.h"
#include "renderer.h"


struct Button
{
	// utility
	void update();
	void remove();

	// data
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
	// utility
	void update(bool field_switch);
	void remove();

	// data
	Sprite* canvas;
	PixelBufferComponent* idle;
	PixelBufferComponent* hover;
	PixelBufferComponent* select;
	string buffer;
	lptr<Text> content;
	Rect bounds;
	bool active = false;
	bool hidden = false;
};

struct UIBatch
{
	// utility
	lptr<Button> add_button(const char* label,PixelBufferComponent* tidle,PixelBufferComponent* thover,
							PixelBufferComponent* taction,vec2 position,vec2 scale,Alignment alignment={});
	lptr<TextField> add_text_field(PixelBufferComponent* tidle,PixelBufferComponent* thover,
								   PixelBufferComponent* tselect,vec2 position,vec2 scale,
								   Alignment alignment={});

	// data
	Font* font;
	f32 alpha;
	list<Button> buttons;
	list<TextField> tfields;
};


class UI
{
public:
	void update();

	// registration/deregistration
	lptr<UIBatch> add_batch(Font* font,f32 alpha=1.f);
	void remove_batch(lptr<UIBatch> batch);

private:
	Font* m_Font;
	list<UIBatch> m_Batches;
};

inline UI g_UI = UI();


#endif
