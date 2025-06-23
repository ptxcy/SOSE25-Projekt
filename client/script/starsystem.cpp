#include "starsystem.h"


/**
 *	starsystem setup routine
 */
StarSystem::StarSystem()
{
	// shader compile
	VertexShader __PlanetVertexShader = VertexShader("core/shader/planet.vert");
	VertexShader __SunVertexShader = VertexShader("core/shader/sun.vert");
	VertexShader __HaloVertexShader = VertexShader("core/shader/halo.vert");
	FragmentShader __PlanetFragmentShader = FragmentShader("core/shader/planet.frag");
	FragmentShader __SunFragmentShader = FragmentShader("core/shader/sun.frag");
	FragmentShader __HaloFragmentShader = FragmentShader("core/shader/halo.frag");
	m_PlanetShader = g_Renderer.register_pipeline(__PlanetVertexShader,__PlanetFragmentShader);
	m_SunShader = g_Renderer.register_pipeline(__SunVertexShader,__SunFragmentShader);
	m_HaloShader = g_Renderer.register_pipeline(__HaloVertexShader,__HaloFragmentShader);

	// load geometry
	Mesh __SphereMesh = Mesh("./res/sphere.obj");
	Mesh __HaloMesh = Mesh("./res/planets/ring.obj");
	Mesh __HaloMeshBS = Mesh("./res/planets/ring_bs.obj");
	__HaloMesh.vertices.insert(__HaloMesh.vertices.end(),
							   __HaloMeshBS.vertices.begin(),__HaloMeshBS.vertices.end());

	// setup planetary geometry
	m_PlanetBatch = g_Renderer.register_particle_batch(m_PlanetShader);
	m_PlanetBatch->load(__SphereMesh,8);

	// load planet textures
	m_PlanetTextures[0] = g_Renderer.register_sprite_texture("./res/planets/halfres/mercury.jpg");
	m_PlanetTextures[1] = g_Renderer.register_sprite_texture("./res/planets/halfres/venus.jpg");
	m_PlanetTextures[2] = g_Renderer.register_sprite_texture("./res/planets/halfres/earth.jpg");
	m_PlanetTextures[3] = g_Renderer.register_sprite_texture("./res/planets/halfres/mars.jpg");
	m_PlanetTextures[4] = g_Renderer.register_sprite_texture("./res/planets/halfres/jupiter.jpg");
	m_PlanetTextures[5] = g_Renderer.register_sprite_texture("./res/planets/halfres/saturn.jpg");
	m_PlanetTextures[6] = g_Renderer.register_sprite_texture("./res/planets/halfres/uranus.jpg");
	m_PlanetTextures[7] = g_Renderer.register_sprite_texture("./res/planets/halfres/neptune.jpg");

	// setup planets
	planets[0] = { vec3(3.9f*STARSYS_DISTANCE_SCALE,0,0),.38f*STARSYS_EARTH_REFERENCE_SCALE };
	planets[1] = { vec3(7.2f*STARSYS_DISTANCE_SCALE,0,0),.95f*STARSYS_EARTH_REFERENCE_SCALE };
	planets[2] = { vec3(10.f*STARSYS_DISTANCE_SCALE,0,0),1.f*STARSYS_EARTH_REFERENCE_SCALE };
	planets[3] = { vec3(15.2f*STARSYS_DISTANCE_SCALE,0,0),.53f*STARSYS_EARTH_REFERENCE_SCALE };
	planets[4] = { vec3(50.2f*STARSYS_DISTANCE_SCALE,0,0),11.21f*STARSYS_EARTH_REFERENCE_SCALE };
	planets[5] = { vec3(95.4f*STARSYS_DISTANCE_SCALE,0,0),9.46f*STARSYS_EARTH_REFERENCE_SCALE };
	planets[6] = { vec3(192.f*STARSYS_DISTANCE_SCALE,0,0),4.01f*STARSYS_EARTH_REFERENCE_SCALE };
	planets[7] = { vec3(300.6f*STARSYS_DISTANCE_SCALE,0,0),3.89f*STARSYS_EARTH_REFERENCE_SCALE };

	// setup planetary halo
	m_HaloTexture = g_Renderer.register_sprite_texture("./res/planets/saturn_ring.png");
	m_HaloBatch = g_Renderer.register_particle_batch(m_HaloShader);
	m_HaloBatch->load(__HaloMesh,1);
	// TODO scale ring accordingly: start at 1.11f, ends at 2.33f
	// TODO join batch definition methods

	COMM_LOG("setup starsystem batches");
	lptr<GeometryBatch> __SunBatch = g_Renderer.register_geometry_batch(m_SunShader);

	// setup sun geometry
	COMM_LOG("load sun geometry and textures");
	vector<Texture*> __SunTextures = { g_Renderer.register_texture("./res/planets/halfres/sun.jpg") };
	u32 __Sun0 = __SunBatch->add_geometry(__SphereMesh,__SunTextures);
	u32 __Sun1 = __SunBatch->add_geometry(__SphereMesh,__SunTextures);
	__SunBatch->load();
	m_SunShader->upload("scale",STARSYS_SUN_REFERENCE_SCALE);
	// TODO model scaling sun: 109.32f

	// register routine
	g_Wheel.call(UpdateRoutine{ &StarSystem::_update,(void*)this });
}

/**
 *	starsystem update routine
 */
void StarSystem::update()
{
	// set planetary texture location
	for (u8 i=0;i<8;i++) planets[i].texture = *m_PlanetTextures[i];

	// set halo data
	m_Halos[0].offset = planets[5].offset;
	m_Halos[0].scale = planets[5].scale*STARSYS_SATURN_RING_DISTFACTOR;
	m_Halos[0].texture = *m_HaloTexture;

	// planetary position update
	m_PlanetBatch->ibo.bind();
	m_PlanetBatch->ibo.upload_vertices(planets,8,GL_DYNAMIC_DRAW);
	m_HaloBatch->ibo.bind();
	m_HaloBatch->ibo.upload_vertices(m_Halos,1,GL_DYNAMIC_DRAW);
	// TODO add an option to automate this process when per-frame update is actually needed
}
