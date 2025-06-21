#include "flotilla.h"


/**
 *	setup flotilla rendering
 */
Flotilla::Flotilla()
{
	// shader compile
	VertexShader __SpaceshipVertexShader = VertexShader("core/shader/spaceship.vert");
	FragmentShader __SpaceshipFragmentShader = FragmentShader("core/shader/spaceship.frag");
	m_SpaceshipShader = g_Renderer.register_pipeline(__SpaceshipVertexShader,__SpaceshipFragmentShader);
	// TODO pack this 3D object instancing into default pipeline

	// load geometry
	Mesh __SpaceshipMesh = Mesh("./res/spaceship.obj");

	// setup spaceship batch
	m_SpaceshipTexture = g_Renderer.register_sprite_texture("./res/test.png");
	m_SpaceshipBatch = g_Renderer.register_particle_batch(m_SpaceshipShader);
	m_SpaceshipBatch->load(__SpaceshipMesh,0);

	// register routine
	g_Wheel.call(UpdateRoutine{ &Flotilla::_update,(void*)this });
}

/**
 *	update flotillae
 */
void Flotilla::update()
{
	// upload fleet
	m_SpaceshipBatch->active_particles = spaceships.size();
	for (u32 i=0;i<spaceships.size();i++)
	{
		spaceships[i].scale = 1;
		spaceships[i].texture = *m_SpaceshipTexture;
		// TODO this can be a uniform upload !!BLAZINGLY FAAST!!
	}

	// update fleet positions
	m_SpaceshipBatch->ibo.bind();
	m_SpaceshipBatch->ibo.upload_vertices(spaceships,GL_DYNAMIC_DRAW);

	// update camera matrices
	m_SpaceshipShader->enable();
	m_SpaceshipShader->upload_camera();
}
