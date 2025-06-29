#ifndef SCRIPT_PLANET_FOCUS_HEADER
#define SCRIPT_PLANET_FOCUS_HEADER


#include "../core/renderer.h"
#include "../core/input.h"
#include "../core/wheel.h"


constexpr f32 FOCUS_PLANET_ROTATION = .25f;
constexpr f32 FOCUS_CLOUD_LAG_ROTATION = .9f;
constexpr f32 FOCUS_REFERENCE_ACC = .001f;
constexpr f32 FOCUS_REFERENCE_POWER = .01f;

constexpr f32 FOCUS_MAXIMUM_DISTANCE = 6.f;
constexpr f32 FOCUS_MAXIMUM_DISTANCE_INV = 1.f/FOCUS_MAXIMUM_DISTANCE;
constexpr f32 FOCUS_LOG_STEEPNESS = 7.f;
constexpr f32 FOCUS_LOG_RAMPPOINT = .5f;

constexpr f32 FOCUS_PROJECTION_MIN = 1.f;
constexpr f32 FOCUS_PROJECTION_MAX = 60.f;
constexpr f32 FOCUS_REFSCALE_MIN = .05f;
constexpr f32 FOCUS_REFSCALE_MAX = 1.f;

constexpr f32 FOCUS_UNIT_TO_METERS = 6371000.f;
constexpr f32 FOCUS_METERS_TO_UNITS = 1.f/FOCUS_UNIT_TO_METERS;


class PlanetFocus
{
public:
	PlanetFocus(Font* font);
	static inline void _update(void* r) { PlanetFocus* p = (PlanetFocus*)r; p->update(); }
	void update();

private:

	// ui
	lptr<Text> m_TxHeightDisplay;

	// graphics
	lptr<GeometryBatch> m_PFBatch;
	lptr<GeometryBatch> m_CloudBatch;
	u32 m_Earth;
	u32 m_Reference;
	u32 m_Clouds;

	// camera
	Camera3D m_PlanetCamera = g_Camera;
	vec3 m_RefPosition = vec3(0,1,0);
	f32 m_SurfaceDistance = 2.f;
};


#endif
