#include "planet_focus.h"


/**
 *	setup planet focus mode with orbital zoom
 *	\param font: ui font
 */
PlanetFocus::PlanetFocus(Font* font)
{
	// height display
	m_TxHeightDisplay = g_Renderer.write_text(font,"",vec3(-15,-15,0),15,
											  vec4(1),Alignment{ .align=SCREEN_ALIGN_TOPRIGHT });

	// geometry
	Mesh __SphereMesh = Mesh("./res/sphere.obj");
	Mesh __MonkeyMesh = Mesh("./res/physical/test_obj.obj");

	// textures
	vector<Texture*> __EarthTextures = {
		g_Renderer.register_texture("./res/planets/earth.jpg",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/planets/earth_normal.png"),
		g_Renderer.register_texture("./res/planets/earth_material.png"),
	};
	vector<Texture*> __MonkeyTextures = {
		g_Renderer.register_texture("./res/physical/gold_colour.png",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/physical/gold_normal.png"),
		g_Renderer.register_texture("./res/physical/gold_material.png"),
	};

	// setup geometry
	m_PFBatch = g_Renderer.register_deferred_geometry_batch();
	m_Earth = m_PFBatch->add_geometry(__SphereMesh,__EarthTextures);
	m_Reference = m_PFBatch->add_geometry(__MonkeyMesh,__MonkeyTextures);
	m_PFBatch->load();

	// link uniform
	m_PFBatch->attach_uniform(m_Earth,"proj",&m_PlanetCamera.proj);

	// lighting
	g_Renderer.add_sunlight(vec3(2000,2000,-2000),vec3(1.f),4.f);
	g_Renderer.upload_lighting();

	// view focus setup
	g_Camera.distance = 2.f;
	//g_Camera.roll(-75.f);

	g_Wheel.call(UpdateRoutine{ &PlanetFocus::_update,(void*)this });
}

/**
 *	update planet focus mode
 */
void PlanetFocus::update()
{
	// precalculations
	f32 __ActualDistance = m_SurfaceDistance-1.f;

	// height display
	m_TxHeightDisplay->data = "distance to surface = "
			+std::to_string((u64)(__ActualDistance*FOCUS_UNIT_TO_METERS))+'m';
	m_TxHeightDisplay->align();
	m_TxHeightDisplay->load_buffer();

	// earth animation
	m_PFBatch->object[m_Earth].transform.rotate_z(FOCUS_PLANET_ROTATION);

	// zoom input
	m_SurfaceDistance += (g_Input.keyboard.keys[SDL_SCANCODE_R]-g_Input.keyboard.keys[SDL_SCANCODE_F])
			*(FOCUS_REFERENCE_ACC+g_Input.keyboard.keys[SDL_SCANCODE_LSHIFT]*FOCUS_REFERENCE_POWER);
	m_SurfaceDistance = (m_SurfaceDistance<1.f) ? 1.f : m_SurfaceDistance;

	// planet update
	f32 __DistanceNorm = __ActualDistance*FOCUS_MAXIMUM_DISTANCE_INV;
	f32 __Logistic = 1.f/(1.f+exp(-FOCUS_LOG_STEEPNESS*(__DistanceNorm-FOCUS_LOG_RAMPPOINT)));
	m_PlanetCamera.fov = glm::mix(FOCUS_PROJECTION_MIN,FOCUS_PROJECTION_MAX,__Logistic);
	m_PFBatch->object[m_Reference].transform.scale(glm::mix(FOCUS_REFSCALE_MIN,FOCUS_REFSCALE_MAX,__Logistic));

	// object update
	vec3 __TargetPosition = m_RefPosition*m_SurfaceDistance;
	m_PFBatch->object[m_Reference].transform.translate(__TargetPosition);
	m_PFBatch->object[m_Reference].transform.scale(.1f);

	// camera update
	g_Camera.target = __TargetPosition;
	g_Camera.update();
	m_PlanetCamera.project();
}
// TODO breakdown transform rotation into an interval: [0,360]
