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
	m_SpaceshipBatch = g_Renderer.register_particle_batch(m_SpaceshipShader);
	m_SpaceshipBatch->geometry.resize(__SpaceshipMesh.vertices.size()*RENDERER_VERTEX_SIZE);
	memcpy(&m_SpaceshipBatch->geometry[0],&__SpaceshipMesh.vertices[0],
		   __SpaceshipMesh.vertices.size()*sizeof(Vertex));
	m_SpaceshipBatch->load();
	m_SpaceshipBatch->vertex_count = __SpaceshipMesh.vertices.size();
	m_SpaceshipBatch->active_particles = 0;
	m_SpaceshipShader->upload("tex",RENDERER_TEXTURE_SPRITES);
	m_SpaceshipTexture = g_Renderer.register_sprite_texture("./res/test.png");

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
		spaceships[i].scale = 1.f;
		spaceships[i].texture = *m_SpaceshipTexture;
		// TODO this can be a uniform upload !!BLAZINGLY FAAST!!
	}
	if (spaceships.size())
	{
		vec3 p = spaceships[0].offset;
		COMM_LOG("%f %f %f",p.x,p.y,p.z);
	}

	// update fleet positions
	m_SpaceshipBatch->ibo.bind();
	m_SpaceshipBatch->ibo.upload_vertices(spaceships,GL_DYNAMIC_DRAW);

	// update camera matrices
	m_SpaceshipShader->enable();
	m_SpaceshipShader->upload_camera();
}
