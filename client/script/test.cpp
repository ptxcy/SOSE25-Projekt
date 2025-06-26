#include "test.h"


/**
 *	test scene setup
 */
TestScene::TestScene()
{
	// shaders
	VertexShader __SunVertexShader = VertexShader("core/shader/sun.vert");
	FragmentShader __SunFragmentShader = FragmentShader("core/shader/sun.frag");
	VertexShader __GeometryPassVertexShader = VertexShader("core/shader/gpass.vert");
	FragmentShader __GeometryPassFragmentShader = FragmentShader("core/shader/gpass.frag");
	lptr<ShaderPipeline> __SunShader = g_Renderer.register_pipeline(__SunVertexShader,__SunFragmentShader);
	lptr<ShaderPipeline> __GPassShader
			= g_Renderer.register_pipeline(__GeometryPassVertexShader,__GeometryPassFragmentShader);

	// geometry
	Mesh __SphereMesh = Mesh("./res/sphere.obj");
	Mesh __MonkeyMesh = Mesh("./res/physical/test_obj.obj");
	Mesh __FloorMesh = Mesh("./res/physical/test_floor.obj");

	// textures
	vector<Texture*> __SunTextures = { g_Renderer.register_texture("./res/planets/halfres/sun.jpg") };
	vector<Texture*> __ParquetTextures = {
		g_Renderer.register_texture("./res/physical/paquet_colour.png"),
		g_Renderer.register_texture("./res/physical/paquet_normal.png"),
		g_Renderer.register_texture("./res/physical/paquet_material.png"),
	};
	vector<Texture*> __MarbleTextures = {
		g_Renderer.register_texture("./res/physical/marble_colour.png"),
		g_Renderer.register_texture("./res/physical/marble_normal.png"),
		g_Renderer.register_texture("./res/physical/marble_material.png"),
	};

	// freeform buffer
	lptr<GeometryBatch> __FreeformBatch = g_Renderer.register_geometry_batch(__SunShader);
	__FreeformBatch->add_geometry(__SphereMesh,__SunTextures);
	__FreeformBatch->load();

	// physical buffer
	lptr<GeometryBatch> __PhysicalBatch = g_Renderer.register_deferred_geometry_batch(__GPassShader);
	__PhysicalBatch->add_geometry(__FloorMesh,__ParquetTextures);
	__PhysicalBatch->add_geometry(__MonkeyMesh,__MarbleTextures);
	__PhysicalBatch->load();

	g_Wheel.call(UpdateRoutine{ &TestScene::_update,(void*)this });
}

/**
 *	update test scene
 */
void TestScene::update() {  }
