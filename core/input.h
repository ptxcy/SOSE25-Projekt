#ifndef CORE_INPUT_HEADER
#define CORE_INPUT_HEADER


#include "base.h"


constexpr u16 KEYBOARD_INPUT_RANGE = 285;
constexpr u8 MOUSE_INPUT_RANGE = 5;


struct Keyboard
{
	bool keys[KEYBOARD_INPUT_RANGE] = { false };
};

struct Mouse
{
	vec2 position;
	s32 wheel;
	bool buttons[MOUSE_INPUT_RANGE] = { false };
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
