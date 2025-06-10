#include "starsystem.h"


/**
 *	starsystem setup routine
 */
StarSystem::StarSystem()
{
	// shader compile
	VertexShader __PlanetVertexShader = VertexShader("core/shader/planet.vert");
	VertexShader __SunVertexShader = VertexShader("core/shader/sun.vert");
	//VertexShader __HaloVertexShader = VertexShader("core/shader/halo.vert");
	FragmentShader __PlanetFragmentShader = FragmentShader("core/shader/planet.frag");
	FragmentShader __SunFragmentShader = FragmentShader("core/shader/sun.frag");
	//FragmentShader __HaloFragmentShader = FragmentShader("core/shader/halo.frag");
	m_PlanetShader = g_Renderer.register_mesh_pipeline(__PlanetVertexShader,__PlanetFragmentShader);
	m_SunShader = g_Renderer.register_mesh_pipeline(__SunVertexShader,__SunFragmentShader);
	//m_HaloShader = g_Renderer.register_mesh_pipeline(__HaloVertexShader,__HaloFragmentShader);

	// load geometry
	Mesh __SphereMesh = Mesh("./res/sphere.obj");

	// setup planetary geometry
	m_PlanetBatch = g_Renderer.register_particle_batch(m_PlanetShader);
	m_PlanetBatch->geometry.resize(__SphereMesh.vertices.size()*RENDERER_VERTEX_SIZE);
	memcpy(&m_PlanetBatch->geometry[0],&__SphereMesh.vertices[0],__SphereMesh.vertices.size()*sizeof(Vertex));
	m_PlanetBatch->load();
	m_PlanetBatch->vertex_count = __SphereMesh.vertices.size();
	m_PlanetBatch->active_particles = 8;
	m_PlanetShader->upload("tex",0);

	// load planet textures
	m_PlanetTextures[0] = g_Renderer.register_sprite_texture("./res/planets/mercury.jpg");
	m_PlanetTextures[1] = g_Renderer.register_sprite_texture("./res/planets/venus.jpg");
	m_PlanetTextures[2] = g_Renderer.register_sprite_texture("./res/planets/earth.jpg");
	m_PlanetTextures[3] = g_Renderer.register_sprite_texture("./res/planets/mars.jpg");
	m_PlanetTextures[4] = g_Renderer.register_sprite_texture("./res/planets/jupiter.jpg");
	m_PlanetTextures[5] = g_Renderer.register_sprite_texture("./res/planets/saturn.jpg");
	m_PlanetTextures[6] = g_Renderer.register_sprite_texture("./res/planets/uranus.jpg");
	m_PlanetTextures[7] = g_Renderer.register_sprite_texture("./res/planets/neptune.jpg");

	// setup planets
	planets[0] = { vec3(3.9f,0,0),.7f };
	planets[1] = { vec3(7.2f,0,0),.7f };
	planets[2] = { vec3(10.f,0,0),1.f };
	planets[3] = { vec3(15.2f,0,0),.9f };
	planets[4] = { vec3(50.2f,0,0),12.f };
	planets[5] = { vec3(95.4f,0,0),10.f };
	planets[6] = { vec3(192.f,0,0),7.f };
	planets[7] = { vec3(300.6f,0,0),4.f };

	// setup planetary halo
	/*
	lptr<MeshBatch> __SaturnHalo = g_Renderer.register_mesh_batch(m_HaloShader);
	__SaturnHalo->register_mesh("./res/planets/ring.obj");
	*/

	// setup sun
	/*
	lptr<MeshBatch> __SunBatch = g_Renderer.register_mesh_batch(m_SunShader);
	__SunBatch->register_mesh(__SphereMesh);
	__SunBatch->load();
	*/
	// TODO also combine load if possible

	// register routine
	g_Wheel.call(UpdateRoutine{ &StarSystem::_update,(void*)this });
}

/**
 *	starsystem update routine
 */
void StarSystem::update()
{
	// upload planetary texture location
	for (u8 i=0;i<STARSYS_PLANET_COUNT;i++) planets[i].texture = *m_PlanetTextures[i];

	// planetary position update
	m_PlanetBatch->ibo.bind();
	m_PlanetBatch->ibo.upload_vertices(planets,8,GL_DYNAMIC_DRAW);

	// update camera matrices
	m_PlanetShader->enable();
	m_PlanetShader->upload_camera();
	m_SunShader->enable();
	m_SunShader->upload_camera();
}
