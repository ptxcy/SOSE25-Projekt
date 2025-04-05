#include "core/blitter.h"
#include "core/input.h"


s32 main(s32 argc,char** argv)
{
	bool running = true;
	while(running)
	{
		g_Frame.clear();

		Input::update(running);

		g_Frame.update();
	}

	g_Frame.close();
	return 0;
}
