#ifndef CORE_INPUT_HEADER
#define CORE_INPUT_HEADER


#include "base.h"


constexpr u16 KEYBOARD_INPUT_RANGE = 285;
constexpr u8 MOUSE_INPUT_RANGE = 5;

constexpr u16 KEYBOARD_INPUT_BITPACKING = KEYBOARD_INPUT_RANGE/8+1;
constexpr u8 MOUSE_INPUT_BITPACKING = MOUSE_INPUT_RANGE/8+1;


struct Keyboard
{
	Bytes keys = Bytes(KEYBOARD_INPUT_BITPACKING);
	Bytes triggered_keys = Bytes(KEYBOARD_INPUT_BITPACKING);
};

struct Mouse
{
	vec2 position;
	s32 wheel;
	Bytes buttons = Bytes(MOUSE_INPUT_BITPACKING);
	Bytes triggered_buttons = Bytes(MOUSE_INPUT_BITPACKING);
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
