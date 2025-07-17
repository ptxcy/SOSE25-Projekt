#ifdef PROJECT_PONG
#include "pong.h"


/**
 *	pong setup
 */
Pong::Pong(Font* font,string name)
{
	// camera setup
	g_Camera.distance = 100.f;
	g_Camera.pitch = glm::radians(45.f);

	// shader compile
	VertexShader __BulbVertexShader = VertexShader("core/shader/bulb.vert");
	FragmentShader __BulbFragmentShader = FragmentShader("core/shader/bulb.frag");
	lptr<ShaderPipeline> __BulbShader = g_Renderer.register_pipeline(__BulbVertexShader,__BulbFragmentShader);

	// geometry
	Mesh __Sphere = Mesh::sphere();
	Mesh __Cube = Mesh::cube();
	Mesh __Triangle = Mesh::triangle();

	// colours
	vector<vec3> __BallColour = {
		vec3(.7f,0,0),vec3(.7f,.7f,0),vec3(.7f,0,.7f),vec3(0,.7f,0),vec3(0,.7f,.7f),vec3(0,0,.7f),
		vec3(.25f,.7f,0),vec3(.7f,.25f,0),vec3(.7f,0,.25f),vec3(.25f,0,.7f),vec3(0,.7f,.25f),vec3(0,.25f,.7f),
	};
	vector<vec3> __LightbulbColour = {
		vec3(1,0,0),vec3(0,1,0),vec3(0,0,1),
		vec3(.5f,.5f,0),vec3(.5f,0,.5f),vec3(0,.5f,.5f),
		//vec3(.8f,.4f,.1f),vec3(.9f,.2f,.1f),vec3(.1f,.1f,.8f)
	};

	// textures
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
	vector<Texture*> __ParquetTextures = {
		g_Renderer.register_texture("./res/physical/fabric_colour.png",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/physical/fabric_normal.png"),
		g_Renderer.register_texture("./res/physical/fabric_material.png"),
	};

	// bulb particle buffer
	m_BulbBatch = g_Renderer.register_deferred_particle_batch(__BulbShader);
	m_BulbBatch->load(__Sphere,PONG_LIGHTING_POINTLIGHTS);

	// ball particle buffer
	m_BallBatch = g_Renderer.register_deferred_particle_batch();
	m_BallBatch->load(__Sphere,PONG_BALL_PHYSICAL_COUNT);
	g_Renderer.register_shadow_batch(m_BallBatch);

	// load surface geometry
	m_PhysicalBatch = g_Renderer.register_deferred_geometry_batch();
	u32 __Floor = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall0 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall1 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall2 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);
	u32 __Wall3 = m_PhysicalBatch->add_geometry(__Cube,__ParquetTextures);

	// load player geometry
	m_Player0 = m_PhysicalBatch->add_geometry(__Triangle,__GoldTextures);
	m_Player1 = m_PhysicalBatch->add_geometry(__Triangle,__GoldTextures);
	m_PhysicalBatch->load();
	g_Renderer.register_shadow_batch(m_PhysicalBatch);

	// setup surface geometry
	m_PhysicalBatch->object[__Floor].transform.scale(vec3(PONG_FIELD_SIZE.x,PONG_FIELD_SIZE.y,PONG_FIELD_WIDTH));
	m_PhysicalBatch->object[__Wall0].transform
			.scale(vec3(PONG_FIELD_WIDTH,PONG_FIELD_SIZE.y+PONG_FIELD_WIDTH,PONG_FIELD_DEPTH));
	m_PhysicalBatch->object[__Wall1].transform
			.scale(vec3(PONG_FIELD_WIDTH,PONG_FIELD_SIZE.y+PONG_FIELD_WIDTH,PONG_FIELD_DEPTH));
	m_PhysicalBatch->object[__Wall2].transform
			.scale(vec3(PONG_FIELD_SIZE.x+PONG_FIELD_WIDTH,PONG_FIELD_WIDTH,PONG_FIELD_DEPTH));
	m_PhysicalBatch->object[__Wall3].transform
			.scale(vec3(PONG_FIELD_SIZE.x+PONG_FIELD_WIDTH,PONG_FIELD_WIDTH,PONG_FIELD_DEPTH));
	m_PhysicalBatch->object[__Floor].transform.translate(vec3(0,0,-PONG_FIELD_DEPTH));
	m_PhysicalBatch->object[__Wall0].transform.translate(vec3(PONG_FIELD_SIZE.x,0,0));
	m_PhysicalBatch->object[__Wall1].transform.translate(vec3(-PONG_FIELD_SIZE.x,0,0));
	m_PhysicalBatch->object[__Wall2].transform.translate(vec3(0,PONG_FIELD_SIZE.y,0));
	m_PhysicalBatch->object[__Wall3].transform.translate(vec3(0,-PONG_FIELD_SIZE.y,0));
	// TODO add mesh baking feature to reduce this to singular geometry with singluar draw call

	// set texel density
	m_PhysicalBatch->object[__Floor].texel = PONG_FIELD_TEXEL;

	// setup index buffer object for ball batches
	for (u32 i=0;i<PONG_BALL_PHYSICAL_COUNT;i++)
	{
		m_BallIndices[i].colour = __BallColour[i%__BallColour.size()];
		m_BallIndices[i].material = vec2((i%PONG_BALL_HALF_COUNT)*PONG_BALL_HALF_COUNT_INV,
										 (i%PONG_BALL_PHYSICAL_COUNT)*PONG_BALL_PHYSICAL_COUNT_INV);
	}
	m_BallBatch->ibo.bind();
	m_BallBatch->ibo.upload_vertices(m_BallIndices,PONG_BALL_PHYSICAL_COUNT,GL_DYNAMIC_DRAW);

	// lighting
	g_Renderer.add_sunlight(vec3(75,-50,100),vec3(1,1,1),.25f);
	for (u32 i=0;i<PONG_LIGHTING_POINTLIGHTS;i++)
	{
		vec3 __BulbColour = __LightbulbColour[i%__LightbulbColour.size()];
		m_BulbIndices[i].colour = __BulbColour;
		m_Lights[i] = g_Renderer.add_pointlight(m_BallIndices[i].position,__BulbColour,10.f,1.f,.8f,.24f);
	}
	g_Renderer.add_shadow(vec3(75,-50,100));
	g_Renderer.upload_lighting();

	// light geometry buffer
	m_BulbBatch->ibo.bind();
	m_BallBatch->ibo.upload_vertices(m_BallIndices,PONG_LIGHTING_POINTLIGHTS,GL_DYNAMIC_DRAW);

	// setup text scoreboard
	m_Score0 = g_Renderer.write_text(font,"",vec3(250,-25,0),15,vec4(1),
									 Alignment{ .align=SCREEN_ALIGN_TOPLEFT });
	m_Score1 = g_Renderer.write_text(font,"",vec3(-250,-25,0),15,
									 vec4(1),Alignment{ .align=SCREEN_ALIGN_TOPRIGHT });

	// connection to server
	string lobby_name = "pong-anaconda";
	g_Websocket.connect(NETWORK_HOST,NETWORK_PORT_ADAPTER,NETWORK_PORT_WEBSOCKET,
						name,"wilson",lobby_name,!strcmp(name.c_str(),"owen"));

	COMM_LOG("sleeping for 300ms so socket is ready");
	std::this_thread::sleep_for(std::chrono::milliseconds(300));
	Request::connect(lobby_name);
	COMM_AWT("sleeping so lobby is registered");
	std::this_thread::sleep_for(std::chrono::milliseconds(NETWORK_CONNECTION_STALL));
	COMM_CNF();

	g_Wheel.call(UpdateRoutine{ &Pong::_update,(void*)this });
}

vec3 ctvec(Coordinate& c) { return vec3(c.x,c.y,c.z); }

/**
 *	update pong game
 */
void Pong::update()
{
	// player input
	s8 __Movement = g_Input.keyboard.keys[SDL_SCANCODE_DOWN]-g_Input.keyboard.keys[SDL_SCANCODE_UP];
	if ((__Movement&&!m_Moving)||(!__Movement&&m_Moving))
	{
		Request::player_movement(__Movement);
		m_Moving = !m_Moving;
	}

	// get server updates
	if (!g_Websocket.state_update) return;
	GameObject __GObj = g_Websocket.receive_message();

	// player positions
	Player& p_Player0 = __GObj.players[1];
	Player& p_Player1 = __GObj.players[0];
	vec3 __PlayerScale = vec3(abs(p_Player0.relative_lines[1].b.x),abs(p_Player0.relative_lines[0].b.y),2)
			*PONG_SCALE_FACTOR;
	m_PhysicalBatch->object[m_Player0].transform.scale(__PlayerScale);
	m_PhysicalBatch->object[m_Player0].transform.translate(ctvec(p_Player0.position)*PONG_SCALE_FACTOR);
	m_PhysicalBatch->object[m_Player1].transform.scale(__PlayerScale);
	m_PhysicalBatch->object[m_Player1].transform.translate(ctvec(p_Player1.position)*PONG_SCALE_FACTOR);
	m_PhysicalBatch->object[m_Player1].transform.rotate_z(180.f);

	// ball positions
	for (u32 i=1;i<PONG_BALL_PHYSICAL_COUNT;i++)
		m_BallIndices[i].position = ctvec(__GObj.balls[i+(i-1)*PONG_DIST_JUMP_INV].position)*PONG_SCALE_FACTOR;
	m_BallBatch->ibo.bind();
	m_BallBatch->ibo.upload_vertices(m_BallIndices,PONG_BALL_PHYSICAL_COUNT,GL_DYNAMIC_DRAW);

	// lighting update & bulb positions
	for (u32 i=0;i<PONG_LIGHTING_POINTLIGHTS;i++)
	{
		vec3 __LightPosition = ctvec(__GObj.balls[i*PONG_DIST_JUMP].position)*PONG_SCALE_FACTOR;
		m_BulbIndices[i].position = __LightPosition;
		m_Lights[i]->position = __LightPosition;
	}
	m_BulbBatch->ibo.bind();
	m_BulbBatch->ibo.upload_vertices(m_BulbIndices,PONG_LIGHTING_POINTLIGHTS);
	g_Renderer.upload_lighting();

	// update scoreboard
	m_Score0->data = "Score: "+std::to_string(__GObj.score.player1);
	m_Score0->align();
	m_Score0->load_buffer();
	m_Score1->data = "Score: "+std::to_string(__GObj.score.player2);
	m_Score1->align();
	m_Score1->load_buffer();
}


#endif
