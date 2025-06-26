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

	// sun information
	vec3 m_SunPosition0 = vec3(2,2,0);
	vec3 m_SunPosition1 = vec3(-3,-4,0);
	vec3 m_SunPosition2 = vec3(-4.5,7,-1);

	// object positions
	vec3 m_BasePosition = vec3(0,0,0);
	vec3 m_ObjPosition0 = vec3(0,0,0);
	vec3 m_ObjPosition1 = vec3(-4,0,0);
	vec3 m_ObjPosition2 = vec3(-2,3,0);
};


#endif
