#ifdef PROJECT_PONG
#include "pong.h"


/**
 *	pong setup
 */
Pong::Pong(string name)
{
	// camera setup
	g_Camera.distance = 100.f;
	g_Camera.pitch = glm::radians(45.f);

	// geometry
	Mesh __Sphere = Mesh("./res/low_sphere.obj");
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
	m_BallBatch = g_Renderer.register_deferred_particle_batch();
	m_BallBatch->load(__Sphere,PONG_BALL_COUNT);
	/*
	m_FreeformBatch = g_Renderer.register_geometry_batch(__BallShader);
	m_Ball0 = m_FreeformBatch->add_geometry(__Sphere,__BallTexture0);
	m_Ball1 = m_FreeformBatch->add_geometry(__Sphere,__BallTexture1);
	m_Ball2 = m_FreeformBatch->add_geometry(__Sphere,__BallTexture2);
	m_FreeformBatch->load();
	*/

	// attach lightsource transforms
	/*
	m_FreeformBatch->attach_uniform(m_Ball0,"offset",&m_BallPosition0);
	m_FreeformBatch->attach_uniform(m_Ball1,"offset",&m_BallPosition1);
	m_FreeformBatch->attach_uniform(m_Ball2,"offset",&m_BallPosition2);
	*/

	// load surface geometry
	m_PhysicalBatch = g_Renderer.register_deferred_geometry_batch();
	u32 __Floor = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall0 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall1 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall2 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall3 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);

	// load player geometry
	/*
	m_Player0 = m_PhysicalBatch->add_geometry(__Cube,__GoldTextures);
	m_Player1 = m_PhysicalBatch->add_geometry(__Cube,__GoldTextures);
	*/
	m_PhysicalBatch->load();

	// setup surface geometry
	m_PhysicalBatch->object[__Floor].transform.scale(vec3(PONG_FIELD_SIZE.x,PONG_FIELD_SIZE.y,PONG_FIELD_WIDTH));
	m_PhysicalBatch->object[__Wall0].transform
			.scale(vec3(PONG_FIELD_WIDTH,PONG_FIELD_SIZE.y+PONG_FIELD_WIDTH,.65f));
	m_PhysicalBatch->object[__Wall1].transform
			.scale(vec3(PONG_FIELD_WIDTH,PONG_FIELD_SIZE.y+PONG_FIELD_WIDTH,.65f));
	m_PhysicalBatch->object[__Wall2].transform
			.scale(vec3(PONG_FIELD_SIZE.x+PONG_FIELD_WIDTH,PONG_FIELD_WIDTH,.65f));
	m_PhysicalBatch->object[__Wall3].transform
			.scale(vec3(PONG_FIELD_SIZE.x+PONG_FIELD_WIDTH,PONG_FIELD_WIDTH,.65f));
	m_PhysicalBatch->object[__Floor].transform.translate(vec3(0,0,-1.15f));
	m_PhysicalBatch->object[__Wall0].transform.translate(vec3(PONG_FIELD_SIZE.x,0,-.5f));
	m_PhysicalBatch->object[__Wall1].transform.translate(vec3(-PONG_FIELD_SIZE.x,0,-.5f));
	m_PhysicalBatch->object[__Wall2].transform.translate(vec3(0,PONG_FIELD_SIZE.y,-.5f));
	m_PhysicalBatch->object[__Wall3].transform.translate(vec3(0,-PONG_FIELD_SIZE.y,-.5f));
	// TODO add mesh baking feature to reduce this to singular geometry with singluar draw call

	// setup player geometry
	/*
	m_PhysicalBatch->object[m_Player0].transform.scale(vec3(.25f,2,1));
	m_PhysicalBatch->object[m_Player1].transform.scale(vec3(.25f,2,1));
	*/

	// set texel density
	m_PhysicalBatch->object[__Floor].texel = PONG_FIELD_TEXEL;

	// lighting
	g_Renderer.add_sunlight(vec3(75,-50,100),vec3(1,1,1),.2f);
	/*
	m_Light0 = g_Renderer.add_pointlight(m_BallPosition0,vec3(.8f,.4f,.1f),10.f,1.f,.8f,.24f);
	m_Light1 = g_Renderer.add_pointlight(m_BallPosition1,vec3(.1f,.1f,.8f),10.f,1.f,.8f,.24f);
	m_Light2 = g_Renderer.add_pointlight(m_BallPosition2,vec3(.9f,.2f,.1f),10.f,1.f,.8f,.24f);
	*/
	g_Renderer.upload_lighting();

	// fucking msgpack fuck my ass
#ifdef FEAT_MULTIPLAYER
	g_Websocket.connect(NETWORK_HOST,NETWORK_PORT_ADAPTER,NETWORK_PORT_WEBSOCKET,
						name,"wilson","pong-anaconda","movie",false);
	Request::connect();
#endif

	// setup index buffer object for ball batch
	m_BallBatch->ibo.bind();
	m_BallBatch->ibo.upload_vertices(m_BallPositions,PONG_BALL_COUNT,GL_DYNAMIC_DRAW);

	g_Wheel.call(UpdateRoutine{ &Pong::_update,(void*)this });
}

/**
 *	update pong game
 */
void Pong::update()
{
	// player input
	/*
	m_PlayerPosition0.y += (g_Input.keyboard.keys[SDL_SCANCODE_W]-g_Input.keyboard.keys[SDL_SCANCODE_S])
			*PONG_PEDAL_ACCELERATION;
	m_PlayerPosition0.y = glm::clamp(m_PlayerPosition0.y,-8.5f,8.5f);
	*/
	// TODO upload input to calculation unit instead and receive resulting pedal positions from server update

	// player position
	/*
	m_PhysicalBatch->object[m_Player0].transform.translate(m_PlayerPosition0);
	m_PhysicalBatch->object[m_Player1].transform.translate(m_PlayerPosition1);
	*/

	// get server updates
	GameObject gobj = g_Websocket.receive_message();
	if (!gobj.balls.size()) return;

	// ball positions
	for (u32 i=0;i<gobj.balls.size();i++)
		m_BallPositions[i].position
				= vec3(gobj.balls[i].position.x,gobj.balls[i].position.y,gobj.balls[i].position.z)*.1f;
	m_BallBatch->ibo.bind();
	m_BallBatch->ibo.upload_vertices(m_BallPositions,PONG_BALL_COUNT,GL_DYNAMIC_DRAW);

	/*
	// lighting update
	m_Light0->position = m_BallPosition0;
	m_Light1->position = m_BallPosition1;
	m_Light2->position = m_BallPosition2;
	*/
	g_Renderer.upload_lighting();
}


#endif
