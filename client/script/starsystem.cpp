#include "starsystem.h"


/**
 *	starsystem setup routine
 */
StarSystem::StarSystem()
{
	// shader compile
	VertexShader __MeshVertexShader = VertexShader("core/shader/planet.vert");
	FragmentShader __MeshFragmentShader = FragmentShader("core/shader/planet.frag");
	m_Shader = g_Renderer.register_mesh_pipeline(__MeshVertexShader,__MeshFragmentShader);

	// setup planetary geometry
	m_Batch = g_Renderer.register_particle_batch(m_Shader);
	m_Batch->load("./res/sphere.obj");
	m_Batch->active_particles = 8;

	// shader config
	m_Shader->upload("tex",0);
	m_Shader->upload_camera();

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
	m_Batch->ibo.bind();
	m_Batch->ibo.upload_vertices(m_Planets,GL_DYNAMIC_DRAW);

	// update camera matrices
	m_Shader->enable();
	m_Shader->upload_camera();
}
