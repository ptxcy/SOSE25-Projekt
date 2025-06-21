#ifndef SCRIPT_STARSYSTEM_HEADER
#define SCRIPT_STARSYSTEM_HEADER


#include "../core/renderer.h"
#include "../core/wheel.h"


constexpr u8 STARSYS_PLANET_COUNT = 8;
constexpr f32 STARSYS_SUN_REFERENCE_SCALE = 15.f;
constexpr f32 STARSYS_EARTH_REFERENCE_SCALE = .5f;
constexpr f32 STARSYS_SATURN_RING_DISTFACTOR = 1.25f;
constexpr f32 STARSYS_DISTANCE_SCALE = 75.f;


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
	vector<PlanetaryAttribute> planets = vector<PlanetaryAttribute>(STARSYS_PLANET_COUNT);
	PixelBufferComponent* m_PlanetTextures[STARSYS_PLANET_COUNT];

private:

	// graphics
	lptr<ShaderPipeline> m_PlanetShader;
	lptr<ShaderPipeline> m_SunShader;
	lptr<ShaderPipeline> m_HaloShader;

	lptr<ParticleBatch> m_PlanetBatch;
	lptr<ParticleBatch> m_HaloBatch;

	// planetary data
	PixelBufferComponent* m_HaloTexture;

	// batch data
	PlanetaryAttribute m_Halos[1];
};


#endif
