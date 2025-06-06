#ifndef SCRIPT_STARSYSTEM_HEADER
#define SCRIPT_STARSYSTEM_HEADER


#include "../core/renderer.h"
#include "../core/wheel.h"


struct PlanetaryAttribute
{
	vec3 offset = vec3(0);
	f32 scale = 1.f;
};


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
	lptr<ParticleBatch> m_Batch;

	// planetary data
	vector<PlanetaryAttribute> m_Planets;
};


#endif
