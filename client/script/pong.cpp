#include "pong.h"


/**
 *	pong setup
 */
Pong::Pong()
{
	// camera setup
	// TODO

	// shaders
	VertexShader __BallVertexShader = VertexShader("core/shader/sun.vert");
	FragmentShader __BallFragmentShader = FragmentShader("core/shader/sun.frag");
	lptr<ShaderPipeline> __BallShader = g_Renderer.register_pipeline(__BallVertexShader,__BallFragmentShader);

	// geometry
	Mesh __Sphere = Mesh("./res/sphere.obj");
	Mesh __Cube = Mesh("./res/cube.obj");

	// textures
	vector<Texture*> __BallTexture0 = { g_Renderer.register_texture("./res/planets/halfres/sun.jpg") };
	vector<Texture*> __BallTexture1 = { g_Renderer.register_texture("./res/planets/halfres/neptune.jpg") };
	vector<Texture*> __BallTexture2 = { g_Renderer.register_texture("./res/planets/halfres/mars.jpg") };
	vector<Texture*> __ParquetTextures = {
		g_Renderer.register_texture("./res/physical/paquet_colour.png",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/physical/paquet_normal.png"),
		g_Renderer.register_texture("./res/physical/paquet_material.png"),
	};
	vector<Texture*> __GoldTextures = {
		g_Renderer.register_texture("./res/physical/gold_colour.png",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/physical/gold_normal.png"),
		g_Renderer.register_texture("./res/physical/gold_material.png"),
	};

	// freeform buffer
	lptr<GeometryBatch> __FreeformBatch = g_Renderer.register_geometry_batch(__BallShader);
	u32 s0 = __FreeformBatch->add_geometry(__Sphere,__BallTexture0);
	u32 s1 = __FreeformBatch->add_geometry(__Sphere,__BallTexture1);
	u32 s2 = __FreeformBatch->add_geometry(__Sphere,__BallTexture2);
	__FreeformBatch->load();

	// attach lightsource transforms
	__FreeformBatch->attach_uniform(s0,"offset",&m_BallPosition0);
	__FreeformBatch->attach_uniform(s1,"offset",&m_BallPosition1);
	__FreeformBatch->attach_uniform(s2,"offset",&m_BallPosition2);

	m_PhysicalBatch = g_Renderer.register_deferred_geometry_batch();
	u32 __Floor = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall0 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall1 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall2 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall3 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	m_Player0 = m_PhysicalBatch->add_geometry(__Cube,__GoldTextures);
	m_Player1 = m_PhysicalBatch->add_geometry(__Cube,__GoldTextures);
	m_PhysicalBatch->load();

	// setup positions
	m_PhysicalBatch->object[__Floor].transform.scale(vec3(PONG_FIELD_SIZE.x,PONG_FIELD_SIZE.y,PONG_FIELD_WIDTH));
	m_PhysicalBatch->object[__Wall0].transform
		.scale(vec3(PONG_FIELD_SIZE.x+PONG_FIELD_WIDTH,PONG_FIELD_WIDTH,.65f));
	m_PhysicalBatch->object[__Wall1].transform
		.scale(vec3(PONG_FIELD_SIZE.x+PONG_FIELD_WIDTH,PONG_FIELD_WIDTH,.65f));
	m_PhysicalBatch->object[__Wall2].transform
		.scale(vec3(PONG_FIELD_WIDTH,PONG_FIELD_SIZE.y+PONG_FIELD_WIDTH,.65f));
	m_PhysicalBatch->object[__Wall3].transform
		.scale(vec3(PONG_FIELD_WIDTH,PONG_FIELD_SIZE.y+PONG_FIELD_WIDTH,.65f));
	m_PhysicalBatch->object[m_Player0].transform.scale(vec3(.25f,2,1));
	m_PhysicalBatch->object[m_Player1].transform.scale(vec3(.25f,2,1));
	m_PhysicalBatch->object[__Floor].transform.translate(vec3(0,0,-1.15f));
	m_PhysicalBatch->object[__Wall0].transform.translate(vec3(PONG_FIELD_SIZE.x,0,-.5f));
	m_PhysicalBatch->object[__Wall1].transform.translate(vec3(-PONG_FIELD_SIZE.x,0,-.5f));
	m_PhysicalBatch->object[__Wall2].transform.translate(vec3(0,PONG_FIELD_SIZE.y,-.5f));
	m_PhysicalBatch->object[__Wall3].transform.translate(vec3(0,-PONG_FIELD_SIZE.y,-.5f));

	// set texel density
	m_PhysicalBatch->object[__Floor].texel = 5;

	// lighting
	g_Renderer.add_sunlight(vec3(75,-50,100),vec3(1,1,1),.2f);
	g_Renderer.add_pointlight(m_BallPosition0,vec3(.8f,.4f,.1f),10.f,1.f,.8f,.24f);
	g_Renderer.add_pointlight(m_BallPosition1,vec3(.1f,.1f,.8f),10.f,1.f,.8f,.24f);
	g_Renderer.add_pointlight(m_BallPosition2,vec3(.9f,.2f,.1f),10.f,1.f,.8f,.24f);
	g_Renderer.upload_lighting();

	// fucking msgpack fuck my ass
#ifdef FEAT_MULTIPLAYER
	g_Websocket.connect(NETWORK_HOST,NETWORK_PORT_ADAPTER,NETWORK_PORT_WEBSOCKET,
						"owen","wilson","pong-anaconda","movie",false);
	Request::connect();
#endif

	g_Wheel.call(UpdateRoutine{ &Pong::_update,(void*)this });
}

/**
 *	update pong game
 */
void Pong::update()
{
	// player input
	m_PlayerPosition0.y += (g_Input.keyboard.keys[SDL_SCANCODE_W]-g_Input.keyboard.keys[SDL_SCANCODE_S])
			*PONG_PEDAL_ACCELERATION;
	m_PlayerPosition0.y = glm::clamp(m_PlayerPosition0.y,-8.5f,8.5f);

	// player position
	m_PhysicalBatch->object[m_Player0].transform.translate(m_PlayerPosition0);
	m_PhysicalBatch->object[m_Player1].transform.translate(m_PlayerPosition1);

	// lighting update
	// TODO
}
