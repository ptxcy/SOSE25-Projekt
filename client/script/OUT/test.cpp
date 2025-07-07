#include "test.h"


/**
 *	test scene setup
 */
TestScene::TestScene()
{
	// camera setup
	g_Camera.distance = 7.f;
	g_Camera.pitch = glm::radians(10.f);

	// shaders
	VertexShader __SunVertexShader = VertexShader("core/shader/sun.vert");
	FragmentShader __SunFragmentShader = FragmentShader("core/shader/sun.frag");
	lptr<ShaderPipeline> __SunShader = g_Renderer.register_pipeline(__SunVertexShader,__SunFragmentShader);

	// geometry
	Mesh __SphereMesh = Mesh("./res/sphere.obj");
	Mesh __MonkeyMesh = Mesh("./res/physical/test_obj.obj");
	Mesh __FloorMesh = Mesh("./res/physical/test_floor.obj");

	// textures
	vector<Texture*> __SunTexture0 = { g_Renderer.register_texture("./res/planets/halfres/sun.jpg") };
	vector<Texture*> __SunTexture1 = { g_Renderer.register_texture("./res/planets/halfres/neptune.jpg") };
	vector<Texture*> __SunTexture2 = { g_Renderer.register_texture("./res/planets/halfres/mars.jpg") };
	vector<Texture*> __ParquetTextures = {
		g_Renderer.register_texture("./res/physical/paquet_colour.png",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/physical/paquet_normal.png"),
		g_Renderer.register_texture("./res/physical/paquet_material.png"),
	};
	vector<Texture*> __MarbleTextures = {
		g_Renderer.register_texture("./res/physical/marble_colour.png",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/physical/marble_normal.png"),
		g_Renderer.register_texture("./res/physical/marble_material.png"),
	};
	vector<Texture*> __GoldTextures = {
		g_Renderer.register_texture("./res/physical/gold_colour.png",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/physical/gold_normal.png"),
		g_Renderer.register_texture("./res/physical/gold_material.png"),
	};
	vector<Texture*> __FabricTextures = {
		g_Renderer.register_texture("./res/physical/fabric_colour.png",TEXTURE_FORMAT_SRGB),
		g_Renderer.register_texture("./res/physical/fabric_normal.png"),
		g_Renderer.register_texture("./res/physical/fabric_material.png"),
	};

	// freeform buffer
	lptr<GeometryBatch> __FreeformBatch = g_Renderer.register_geometry_batch(__SunShader);
	u32 s0 = __FreeformBatch->add_geometry(__SphereMesh,__SunTexture0);
	u32 s1 = __FreeformBatch->add_geometry(__SphereMesh,__SunTexture1);
	u32 s2 = __FreeformBatch->add_geometry(__SphereMesh,__SunTexture2);
	__FreeformBatch->load();

	// attach lightsource transforms
	__FreeformBatch->attach_uniform(s0,"offset",&m_SunPosition0);
	__FreeformBatch->attach_uniform(s1,"offset",&m_SunPosition1);
	__FreeformBatch->attach_uniform(s2,"offset",&m_SunPosition2);

	// physical buffer
	lptr<GeometryBatch> __PhysicalBatch = g_Renderer.register_deferred_geometry_batch();
	u32 t0 = __PhysicalBatch->add_geometry(__FloorMesh,__ParquetTextures);
	u32 o0 = __PhysicalBatch->add_geometry(__MonkeyMesh,__MarbleTextures);
	u32 o1 = __PhysicalBatch->add_geometry(__MonkeyMesh,__GoldTextures);
	u32 o2 = __PhysicalBatch->add_geometry(__SphereMesh,__FabricTextures);
	__PhysicalBatch->load();

	// attach physical transforms
	__PhysicalBatch->object[t0].transform.translate(m_BasePosition);
	__PhysicalBatch->object[o0].transform.translate(m_ObjPosition0);
	__PhysicalBatch->object[o1].transform.translate(m_ObjPosition1);
	__PhysicalBatch->object[o2].transform.translate(m_ObjPosition2);

	// set texel density
	__PhysicalBatch->object[t0].texel = 5;
	__PhysicalBatch->object[o0].texel = 5;
	__PhysicalBatch->object[o1].texel = 5;
	__PhysicalBatch->object[o2].texel = 2;

	// add light
	g_Renderer.add_pointlight(m_SunPosition0,vec3(.8f,.4f,.1f),10.f,1.f,.8f,.24f);
	g_Renderer.add_pointlight(m_SunPosition1,vec3(.1f,.1f,.8f),10.f,1.f,.8f,.24f);
	g_Renderer.add_pointlight(m_SunPosition2,vec3(.9f,.2f,.1f),10.f,1.f,.8f,.24f);
	//g_Renderer.add_sunlight(vec3(20,20,-20),vec3(1.f),1.f);
	g_Renderer.upload_lighting();

	g_Wheel.call(UpdateRoutine{ &TestScene::_update,(void*)this });
}

/**
 *	update test scene
 */
void TestScene::update() {  }
