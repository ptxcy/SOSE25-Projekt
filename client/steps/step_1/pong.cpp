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

	// geometry
	Mesh __Cube = Mesh::cube();
	Mesh __Triangle = Mesh::triangle();

	// textures
	vector<Texture*> __FloorTextures = {
		g_Renderer.register_texture("./res/pong/floor_colour.png",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/pong/floor_normal.png"),
		g_Renderer.register_texture("./res/pong/floor_material.png"),
	};
	vector<Texture*> __PedalTextures = {
		g_Renderer.register_texture("./res/pong/pedal_colour.png",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/pong/pedal_normal.png"),
		g_Renderer.register_texture("./res/pong/pedal_material.png"),
	};

	// load surface geometry
	m_PhysicalBatch = g_Renderer.register_deferred_geometry_batch();
	u32 __Floor = m_PhysicalBatch->add_geometry(__Cube,__FloorTextures);
	u32 __Wall0 = m_PhysicalBatch->add_geometry(__Cube,__FloorTextures);
	u32 __Wall1 = m_PhysicalBatch->add_geometry(__Cube,__FloorTextures);
	u32 __Wall2 = m_PhysicalBatch->add_geometry(__Cube,__FloorTextures);
	u32 __Wall3 = m_PhysicalBatch->add_geometry(__Cube,__FloorTextures);

	// load player geometry
	m_Player0 = m_PhysicalBatch->add_geometry(__Triangle,__PedalTextures);
	m_Player1 = m_PhysicalBatch->add_geometry(__Triangle,__PedalTextures);
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

	// set texel density
	m_PhysicalBatch->object[__Floor].texel = PONG_FIELD_TEXEL;
	m_PhysicalBatch->object[m_Player0].texel = PONG_FIELD_TEXEL*.25f;
	m_PhysicalBatch->object[m_Player1].texel = PONG_FIELD_TEXEL*.25f;

	// lighting
	g_Renderer.add_sunlight(vec3(75,-50,100),vec3(1,1,1),.25f);
	g_Renderer.add_shadow(vec3(75,-50,100));
	g_Renderer.upload_lighting();

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

	// update scoreboard
	m_Score0->data = "Score: "+std::to_string(__GObj.score.player1);
	m_Score0->align();
	m_Score0->load_buffer();
	m_Score1->data = "Score: "+std::to_string(__GObj.score.player2);
	m_Score1->align();
	m_Score1->load_buffer();
}


#endif
