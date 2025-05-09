#ifndef CORE_INPUT_HEADER
#define CORE_INPUT_HEADER


#include "base.h"


constexpr u16 KEYBOARD_INPUT_RANGE = 285;
constexpr u8 MOUSE_INPUT_RANGE = 5;


struct Keyboard
{
	BitwiseWords keys = BitwiseWords(KEYBOARD_INPUT_RANGE);
	BitwiseWords triggered_keys = BitwiseWords(KEYBOARD_INPUT_RANGE);
};

struct Mouse
{
	s32 apos_x,apos_y;
	vec2 position;
	s32 wheel;
	BitwiseWords buttons = BitwiseWords(MOUSE_INPUT_RANGE);
	BitwiseWords triggered_buttons = BitwiseWords(MOUSE_INPUT_RANGE);
};


class Input
{
public:
	void update(bool& running);

public:
	static inline Keyboard keyboard;
	static inline Mouse mouse;

private:
	SDL_Event m_Event;
};

inline Input g_Input;


#endif
