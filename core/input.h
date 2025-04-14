#ifndef CORE_INPUT_HEADER
#define CORE_INPUT_HEADER


#include "base.h"


constexpr u16 KEYBOARD_INPUT_RANGE = 285;
constexpr u8 MOUSE_INPUT_RANGE = 5;

constexpr u16 KEYBOARD_INPUT_BITPACKING = KEYBOARD_INPUT_RANGE/8+1;
constexpr u8 MOUSE_INPUT_BITPACKING = MOUSE_INPUT_RANGE/8+1;


struct Keyboard
{
	u8 keys[KEYBOARD_INPUT_BITPACKING] = { false };
	u8 triggered_keys[KEYBOARD_INPUT_BITPACKING] = { false };
};

struct Mouse
{
	vec2 position;
	s32 wheel;
	u8 buttons[MOUSE_INPUT_BITPACKING] = { false };
	u8 triggered_buttons[MOUSE_INPUT_BITPACKING] = { false };
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
