#ifndef SCRIPT_STARSYSTEM_HEADER
#define SCRIPT_STARSYSTEM_HEADER


#include "../core/renderer.h"
#include "../core/wheel.h"


constexpr u8 STARSYS_PLANET_COUNT = 8;
constexpr f32 STARSYS_SATURN_RING_DISTFACTOR = 1.25f;


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

public:
	PlanetaryAttribute planets[STARSYS_PLANET_COUNT];
	// FIXME why not private again??

private:

	// graphics
	lptr<ShaderPipeline> m_PlanetShader;
	lptr<ShaderPipeline> m_SunShader;
	lptr<ShaderPipeline> m_HaloShader;

	lptr<ParticleBatch> m_PlanetBatch;
	lptr<ParticleBatch> m_HaloBatch;

	// planetary data
	PixelBufferComponent* m_PlanetTextures[STARSYS_PLANET_COUNT];
	PixelBufferComponent* m_HaloTexture;

	// batch data
	PlanetaryAttribute m_Halos[1];
};


#endif
