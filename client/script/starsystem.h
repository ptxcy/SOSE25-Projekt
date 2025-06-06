#ifndef SCRIPT_STARSYSTEM_HEADER
#define SCRIPT_STARSYSTEM_HEADER


#include "../core/renderer.h"
#include "../core/wheel.h"


class StarSystem
{
public:
	StarSystem();
	static inline void _update(void* starsystem) { StarSystem* p = (StarSystem*)starsystem; p->update(); }
	void update();
	void close();

private:

	// graphics
	lptr<ShaderPipeline> m_Shader;
};


#endif
