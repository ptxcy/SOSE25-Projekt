# Getting Started

## Basic Setup

```c++
#include "core/blitter.h"
#include "core/input.h"
#include "core/renderer.h"

s32 main(s32 argc,char** argv)
{
	// your setup here

	bool running = true;
	while (running)
	{
		g_Frame.clear();
		g_Input.update(running);

		// your update here

		g_Renderer.update();
		g_Frame.update();
	}
	g_Renderer.exit();
	g_Frame.close();
	return 0;
}
```


## Where to go from here

look up how to use the engine features in the "basic features" section [here](../README.md)
