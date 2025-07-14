#ifndef SCRIPT_FLOTILLA_HEADER
#define SCRIPT_FLOTILLA_HEADER
#ifdef PROJECT_SPACER


#include "../core/renderer.h"
#include "../core/wheel.h"
#include "../adapter/definition.h"


struct SpaceshipData
{
	vec3 offset;
	f32 scale;
	PixelBufferComponent texture;
};


class Flotilla
{
public:
	Flotilla();
	static inline void _update(void* flotilla) { Flotilla* p = (Flotilla*)flotilla; p->update(); }
	void update();

public:
	std::map<u64,Spaceship> fleet;
	vector<SpaceshipData> spaceships;

private:
	lptr<ShaderPipeline> m_SpaceshipShader;
	lptr<ParticleBatch> m_SpaceshipBatch;
	PixelBufferComponent* m_SpaceshipTexture;
};


#endif
#endif
