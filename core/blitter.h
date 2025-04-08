#ifndef CORE_BLITTER_HEADER
#define CORE_BLITTER_HEADER


#include "base.h"


class Frame
{
public:
	Frame(const char* title,u16 width,u16 height,bool vsync=true);

	static void clear();
	void update();
	void close();

	void gpu_vsync_on();
	void gpu_vsync_off();

private:
	SDL_Window* m_Frame;
	SDL_GLContext m_Context;
};

inline Frame g_Frame = Frame("BHT Projekt",FRAME_RESOLUTION_X,FRAME_RESOLUTION_Y);


#endif
