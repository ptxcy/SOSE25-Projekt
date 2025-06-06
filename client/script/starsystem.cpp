#include "starsystem.h"


/**
 *	starsystem setup routine
 */
StarSystem::StarSystem()
{
	// shader compile
	VertexShader __MeshVertexShader = VertexShader("core/shader/planet.vert");
	FragmentShader __MeshFragmentShader = FragmentShader("core/shader/planet.frag");
	m_Shader = g_Renderer.register_mesh_pipeline(__MeshVertexShader,__MeshFragmentShader);

	// setup planetary geometry
	lptr<MeshBatch> __Batch = g_Renderer.register_mesh_batch(m_Shader);
	__Batch->register_mesh("./res/sphere.obj");
	__Batch->load();

	// shader config
	m_Shader->upload("tex",0);
	m_Shader->upload_camera();

	// register routine
	g_Wheel.call(UpdateRoutine{ &StarSystem::_update,(void*)this });
}

/**
 *	starsystem update routine
 */
void StarSystem::update()
{
	// update camera matrices
	m_Shader->enable();
	m_Shader->upload_camera();
}
