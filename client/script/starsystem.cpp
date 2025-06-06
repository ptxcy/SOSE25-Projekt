#include "starsystem.h"


/**
 *	starsystem setup routine
 */
StarSystem::StarSystem()
{
	// shader compile
	VertexShader __PlanetVertexShader = VertexShader("core/shader/planet.vert");
	VertexShader __SunVertexShader = VertexShader("core/shader/sun.vert");
	FragmentShader __PlanetFragmentShader = FragmentShader("core/shader/planet.frag");
	FragmentShader __SunFragmentShader = FragmentShader("core/shader/sun.frag");
	m_PlanetShader = g_Renderer.register_mesh_pipeline(__PlanetVertexShader,__PlanetFragmentShader);
	m_SunShader = g_Renderer.register_mesh_pipeline(__SunVertexShader,__SunFragmentShader);

	// setup planetary geometry
	m_PlanetBatch = g_Renderer.register_particle_batch(m_PlanetShader);
	m_PlanetBatch->load("./res/sphere.obj");
	m_PlanetBatch->active_particles = 8;

	lptr<MeshBatch> __SunBatch = g_Renderer.register_mesh_batch(m_SunShader);
	__SunBatch->register_mesh("./res/sphere.obj");
	__SunBatch->load();
	// FIXME loading this twice is dumb, lets not!
	// TODO also combine load if possible

	// shader config
	m_PlanetShader->upload("tex",0);
	m_PlanetShader->upload_camera();

	// setup planets
	m_Planets.resize(8);
	m_Planets[0] = { vec3(3.9f,0,0),.7f };
	m_Planets[1] = { vec3(7.2f,0,0),.7f };
	m_Planets[2] = { vec3(10.f,0,0),1.f };
	m_Planets[3] = { vec3(15.2f,0,0),.9f };
	m_Planets[4] = { vec3(50.2f,0,0),12.f };
	m_Planets[5] = { vec3(95.4f,0,0),10.f };
	m_Planets[6] = { vec3(192.f,0,0),7.f };
	m_Planets[7] = { vec3(300.6f,0,0),4.f };

	// register routine
	g_Wheel.call(UpdateRoutine{ &StarSystem::_update,(void*)this });
}

/**
 *	starsystem update routine
 */
void StarSystem::update()
{
	// planetary position update
	m_PlanetBatch->ibo.bind();
	m_PlanetBatch->ibo.upload_vertices(m_Planets,GL_DYNAMIC_DRAW);

	// update camera matrices
	m_PlanetShader->enable();
	m_PlanetShader->upload_camera();
}
