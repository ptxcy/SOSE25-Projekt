#include "planet_focus.h"


/**
 *	setup planet focus mode with orbital zoom
 */
PlanetFocus::PlanetFocus()
{
	// load resources
	Mesh __SphereMesh = Mesh("./res/sphere.obj");
	vector<Texture*> __EarthTextures = {
		g_Renderer.register_texture("./res/planets/earth.jpg",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/planets/earth_normal.png"),
		g_Renderer.register_texture("./res/planets/earth_material.png"),
	};

	// setup geometry
	m_PFBatch = g_Renderer.register_deferred_geometry_batch();
	m_Earth = m_PFBatch->add_geometry(__SphereMesh,__EarthTextures);
	m_PFBatch->load();

	// lighting
	g_Renderer.add_sunlight(vec3(2000,2000,-2000),vec3(1.f),4.f);
	g_Renderer.upload_lighting();

	g_Wheel.call(UpdateRoutine{ &PlanetFocus::_update,(void*)this });
}

/**
 *	update planet focus mode
 */
void PlanetFocus::update()
{
	m_PFBatch->object[m_Earth].transform.rotate_z(FOCUS_PLANET_ROTATION);
}
// TODO breakdown transform rotation into an interval: [0,360]
