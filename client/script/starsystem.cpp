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
	m_PlanetBatch->geometry.resize(__SphereMesh.vertices.size()*RENDERER_VERTEX_SIZE);
	memcpy(&m_PlanetBatch->geometry[0],&__SphereMesh.vertices[0],__SphereMesh.vertices.size()*sizeof(Vertex));
	m_PlanetBatch->load();
	m_PlanetBatch->vertex_count = __SphereMesh.vertices.size();
	m_PlanetBatch->active_particles = 8;
	m_PlanetShader->upload("tex",RENDERER_TEXTURE_SPRITES);

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
	planets[0] = { vec3(3.9f,0,0),.7f };
	planets[1] = { vec3(7.2f,0,0),.7f };
	planets[2] = { vec3(10.f,0,0),1.f };
	planets[3] = { vec3(15.2f,0,0),.9f };
	planets[4] = { vec3(50.2f,0,0),12.f };
	planets[5] = { vec3(95.4f,0,0),10.f };
	planets[6] = { vec3(192.f,0,0),7.f };
	planets[7] = { vec3(300.6f,0,0),4.f };

	// setup planetary halo
	m_HaloBatch = g_Renderer.register_particle_batch(m_HaloShader);
	m_HaloBatch->geometry.resize(__HaloMesh.vertices.size()*RENDERER_VERTEX_SIZE);
	memcpy(&m_HaloBatch->geometry[0],&__HaloMesh.vertices[0],__HaloMesh.vertices.size()*sizeof(Vertex));
	m_HaloBatch->load();
	m_HaloBatch->vertex_count = __HaloMesh.vertices.size();
	m_HaloBatch->active_particles = 1;
	m_HaloShader->upload("tex",RENDERER_TEXTURE_SPRITES);
	m_HaloTexture = g_Renderer.register_sprite_texture("./res/planets/saturn_ring.png");

	lptr<GeometryBatch> __SunBatch = g_Renderer.register_geometry_batch(m_SunShader);
	TextureData __SunPixelData = TextureData();
	__SunPixelData.load("./res/planets/halfres/sun.jpg");
	__SunBatch->textures.resize(1);
	__SunBatch->textures[0].bind(RENDERER_TEXTURE_UNMAPPED);
	__SunPixelData.gpu_upload();
	Texture::set_texture_parameter_linear_mipmap();
	Texture::set_texture_parameter_clamp_to_edge();
	Texture::generate_mipmap();
	__SunBatch->geometry.resize(__SphereMesh.vertices.size()*RENDERER_VERTEX_SIZE);
	memcpy(&__SunBatch->geometry[0],&__SphereMesh.vertices[0],__SphereMesh.vertices.size()*sizeof(Vertex));
	__SunBatch->load();
	__SunBatch->vertex_count = __SphereMesh.vertices.size();
	m_SunShader->upload("tex",RENDERER_TEXTURE_UNMAPPED);
	// TODO also combine load if possible
	// TODO detach texture load to reduce stall when loading

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
	m_Halos[0].offset = planets[5].offset;
	m_Halos[0].scale = planets[5].scale*STARSYS_SATURN_RING_DISTFACTOR;
	m_Halos[0].texture = *m_HaloTexture;

	// planetary position update
	m_PlanetBatch->ibo.bind();
	m_PlanetBatch->ibo.upload_vertices(planets,8,GL_DYNAMIC_DRAW);
	m_HaloBatch->ibo.bind();
	m_HaloBatch->ibo.upload_vertices(m_Halos,1,GL_DYNAMIC_DRAW);

	// update camera matrices
	m_PlanetShader->enable();
	m_PlanetShader->upload_camera();
	m_HaloShader->enable();
	m_HaloShader->upload_camera();
	m_SunShader->enable();
	m_SunShader->upload_camera();
	// FIXME this should not happen like this. automate!
}
