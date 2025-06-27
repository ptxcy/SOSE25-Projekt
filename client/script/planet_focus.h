#ifndef SCRIPT_PLANET_FOCUS_HEADER
#define SCRIPT_PLANET_FOCUS_HEADER


#include "../core/renderer.h"
#include "../core/input.h"
#include "../core/wheel.h"


constexpr f32 FOCUS_PLANET_ROTATION = 2.4f;


class PlanetFocus
{
public:
	PlanetFocus();
	static inline void _update(void* r) { PlanetFocus* p = (PlanetFocus*)r; p->update(); }
	void update();

private:

	lptr<GeometryBatch> m_PFBatch;
	u32 m_Earth;
};


#endif
