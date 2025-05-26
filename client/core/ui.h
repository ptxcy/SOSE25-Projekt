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
	Rect bounds;
	bool holding = false;
	bool confirm = false;
};
constexpr u16 UI_MAXIMUM_BUTTON_COUNT = 16;

struct UIBatch
{
	// utility
	Button* add_button(string label,string tidle,string thover,string taction,vec2 position,vec2 scale);

	// data
	InPlaceArray<Button> buttons = InPlaceArray<Button>(UI_MAXIMUM_BUTTON_COUNT);
};


class UI
{
public:
	void update();

public:
	vector<UIBatch*> batches = vector<UIBatch*>();
};

inline UI g_UI = UI();


#endif
