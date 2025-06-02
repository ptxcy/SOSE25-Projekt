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
	// utility
	inline string& get_content() { return content->data; }

	// data
	Sprite* canvas;
	PixelBufferComponent* idle;
	PixelBufferComponent* hover;
	PixelBufferComponent* select;
	lptr<Text> content;
	Rect bounds;
	bool active = false;
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
	list<Button> buttons;
	list<TextField> tfields;
};


class UI
{
public:
	void update();

	// registration/deregistration
	lptr<UIBatch> add_batch(Font* font);
	void remove_batch(lptr<UIBatch> batch);

private:
	Font* m_Font;
	list<UIBatch> m_Batches;
};

inline UI g_UI = UI();


#endif
