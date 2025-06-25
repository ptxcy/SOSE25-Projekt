#ifndef SCRIPT_TEST_HEADER
#define SCRIPT_TEST_HEADER


#include "../core/renderer.h"
#include "../core/input.h"
#include "../core/wheel.h"


class TestScene
{
public:
	TestScene();
	static inline void _update(void* testscene) { TestScene* p = (TestScene*)testscene; p->update(); }
	void update();

private:

	u32 switcher = 0;
};


#endif
