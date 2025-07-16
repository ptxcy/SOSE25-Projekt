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
	/*
	VertexShader __BulbVertexShader = VertexShader("core/shader/bulb.vert");
	FragmentShader __BulbFragmentShader = FragmentShader("core/shader/bulb.frag");
	ShaderPipeline __BulbShader = g_Renderer.register_pipeline(__BulbVertexShader,__BulbFragmentShader);
	*/

	// geometry
	Mesh __Sphere = Mesh("./res/low_sphere.obj");
	Mesh __Cube = Mesh("./res/cube.obj");
	Mesh __Triangle = Mesh("./res/triangle.obj");

	// textures
	vector<Lightbulb> __LightbulbColour = {
		{ { g_Renderer.register_texture("./res/planets/halfres/sun.jpg") },vec3(.8f,.4f,.1f) },
		{ { g_Renderer.register_texture("./res/planets/halfres/mars.jpg") },vec3(.9f,.2f,.1f) },
		{ { g_Renderer.register_texture("./res/planets/halfres/neptune.jpg") },vec3(.1f,.1f,.8f) },
	};
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

	// bulb particle buffer
	/*
	m_BulbBatch = g_Renderer.register_deferred_particle_batch(__BulbShader);
	m_BulbBatch->load(__Sphere,PONG_LIGHTING_POINTLIGHTS);
	*/

	// ball particle buffer
	m_BallBatch = g_Renderer.register_deferred_particle_batch();
	g_Renderer.register_shadow_batch(m_BallBatch);
	m_BallBatch->load(__Sphere,PONG_BALL_COUNT);

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
	g_Renderer.register_shadow_batch(m_PhysicalBatch);
	m_PhysicalBatch->load();

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

	// lighting
	g_Renderer.add_sunlight(vec3(75,-50,100),vec3(1,1,1),.25f);
	for (u32 i=0;i<PONG_LIGHTING_POINTLIGHTS;i++)
	{
		m_Lights[i] = g_Renderer.add_pointlight(m_BallIndices[i].position,
												__LightbulbColour[i%__LightbulbColour.size()].colour,
												10.f,1.f,.8f,.24f);
	}
	g_Renderer.add_shadow(vec3(50,-50,50));
	g_Renderer.upload_lighting();

	// setup text scoreboard
	m_Score0 = g_Renderer.write_text(font,"",vec3(250,-25,0),15,vec4(1),
									 Alignment{ .align=SCREEN_ALIGN_TOPLEFT });
	m_Score1 = g_Renderer.write_text(font,"",vec3(-250,-25,0),15,
									 vec4(1),Alignment{ .align=SCREEN_ALIGN_TOPRIGHT });

	// connection to server
	string lobby_name = "pong-anaconda";
	g_Websocket.connect(NETWORK_HOST,NETWORK_PORT_ADAPTER,NETWORK_PORT_WEBSOCKET,
						name,"wilson",lobby_name,"movie",!strcmp(name.c_str(),"owen"));
	Request::connect(lobby_name);

	// setup index buffer object for ball batch
	m_BallBatch->ibo.bind();
	m_BallBatch->ibo.upload_vertices(m_BallIndices,PONG_BALL_COUNT,GL_DYNAMIC_DRAW);

	g_Wheel.call(UpdateRoutine{ &Pong::_update,(void*)this });
}

vec3 ctvec(Coordinate& c) { return vec3(c.x,c.y,c.z); }

/**
 *	update pong game
 */
void Pong::update()
{
	// player input
	Request::player_movement(g_Input.keyboard.keys[SDL_SCANCODE_S]-g_Input.keyboard.keys[SDL_SCANCODE_W]);

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
	for (u32 i=0;i<__GObj.balls.size();i++)
		m_BallIndices[i].position
				= vec3(__GObj.balls[i].position.x,__GObj.balls[i].position.y,__GObj.balls[i].position.z)*.1f;
	m_BallBatch->ibo.bind();
	m_BallBatch->ibo.upload_vertices(m_BallIndices,PONG_BALL_COUNT,GL_DYNAMIC_DRAW);

	// lighting update
	for (u32 i=0;i<PONG_LIGHTING_POINTLIGHTS;i++)
		m_Lights[i]->position = m_BallIndices[i].position;
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
