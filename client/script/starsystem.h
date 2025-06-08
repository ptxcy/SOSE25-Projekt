#ifndef SCRIPT_STARSYSTEM_HEADER
#define SCRIPT_STARSYSTEM_HEADER


#include "../core/renderer.h"
#include "../core/wheel.h"


constexpr u8 STARSYS_PLANET_COUNT = 8;


struct PlanetaryAttribute
{
	vec3 offset = vec3(0);
	f32 scale = 1.f;
	PixelBufferComponent texture;
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
	lptr<ShaderPipeline> m_PlanetShader;
	lptr<ShaderPipeline> m_SunShader;
	lptr<ParticleBatch> m_PlanetBatch;

	// planetary data
	PlanetaryAttribute m_Planets[STARSYS_PLANET_COUNT];
	PixelBufferComponent* m_PlanetTextures[STARSYS_PLANET_COUNT];
};


#endif
