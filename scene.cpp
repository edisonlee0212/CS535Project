#include "stdafx.h"

#include "scene.h"

#include <string>


#include "vec3.h"
#include "mat3.h"
#include "Camera.h"
#include "Mesh.h"
#define A4_E2
Scene* scene;
float Scene::_AmbientLight = 0.1f;
vector<DirectionalLight> Scene::_DirectionalLights;
vector<PointLight> Scene::_PointLights;

ThreadPool Scene::_ThreadPool;
vector<Mesh*> Scene::_Meshes;
vector<Model*> Scene::_Models;
Camera* Scene::_MainCamera;
FrameBuffer* Scene::_FrameBuffer;
GUI* Scene::_GUI;
float Scene::_LastTimeStep = 0;
float Scene::_CurrentTime = 0;
vec3 Scene::_CameraDir;
vec3 Scene::_CameraPos;
vec3 Scene::_ProjPos;
vec3 Scene::_ProjDir;
int Scene::_ProjLimit;
Camera* Scene::_ProjCamera;
FrameBuffer* Scene::_ProjBuffer;
Texture* Scene::_ProjTexture;
using namespace std;

#include <iostream>

ThreadPool* Scene::GetThreadPool()
{
	return &_ThreadPool;
}

Scene::Scene()
{
	_GUI = new GUI();
	_GUI->show();
	_ThreadPool.Resize(std::thread::hardware_concurrency());

	int u0 = 20;
	int v0 = 20;
	int h = 720;
	int w = 1280;
	float hfov = 90.0f;
	
#ifdef A4_E2
	h = 360;
	w = 640;
	hfov = 90.0f;
#endif
	
#ifdef A4_2
	h = 360;
	w = 640;
	hfov = 90.0f;
#endif
	
	_FrameBuffer = new FrameBuffer(u0, v0, w, h, 0);
	_FrameBuffer->label("SW frame buffer");
	_FrameBuffer->show();
	_FrameBuffer->redraw();

	_GUI->uiw->position(u0, v0 + h + 50);
	
	_MainCamera = new Camera(hfov, _FrameBuffer->Width, _FrameBuffer->Height);

	_MainCamera->SetPose(vec3(20.0f, 80.0f, 80.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	
	
#ifdef A2
	auto DNA = new Mesh();
	DNA->LoadBin("geometry/DNA.bin");
	DNA->SetCenter(vec3(0.0f, 0.0f, 0.0f));
	DNA->Scale(vec3(50.0f, 50.0f, 50.0f));

	auto teapot0 = new Mesh();
	teapot0->LoadBin("geometry/teapot1K.bin");
	teapot0->SetCenter(vec3(100.0f, 50.0f, 0.0f));
	teapot0->Scale(vec3(0.5f, 0.5f, 0.5f));

	auto teapot1 = new Mesh();
	teapot1->LoadBin("geometry/teapot1K.bin");
	teapot1->SetCenter(vec3(100.0f, 0.0f, 0.0f));
	teapot1->Scale(vec3(0.8f, 0.8f, 0.8f));

	auto teapot2 = new Mesh();
	teapot2->LoadBin("geometry/teapot1K.bin");
	teapot2->SetCenter(vec3(100.0f, -50.0f, 0.0f));
	teapot2->Scale(vec3(1.0f, 1.0f, 1.0f));

	auto happy0 = new Mesh();
	happy0->LoadBin("geometry/happy4.bin");
	happy0->SetCenter(vec3(-100.0f, 50.0f, 0.0f));
	happy0->Scale(vec3(100.0f, 100.0f, 100.0f));

	auto happy1 = new Mesh();
	happy1->LoadBin("geometry/happy4.bin");
	happy1->SetCenter(vec3(-100.0f, 0.0f, 0.0f));
	happy1->Scale(vec3(150.0f, 150.0f, 150.0f));

	auto happy2 = new Mesh();
	happy2->LoadBin("geometry/happy4.bin");
	happy2->SetCenter(vec3(-100.0f, -50.0f, 0.0f));
	happy2->Scale(vec3(200.0f, 200.0f, 200.0f));

	_Meshes.push_back(DNA);
	_Meshes.push_back(teapot0);
	_Meshes.push_back(teapot1);
	_Meshes.push_back(teapot2);
	_Meshes.push_back(happy0);
	_Meshes.push_back(happy1);
	_Meshes.push_back(happy2);
#endif
#ifdef A3
	_DirectionalLights.emplace_back();
	_DirectionalLights[0].diffuse = 0.2f;
	_DirectionalLights[0].specular = 0.7f;
	_DirectionalLights[0].direction = vec3(0, 0, -1);
	
	auto windowmat = std::make_shared<Material>();
	windowmat->SetShininess(4.0f);
	windowmat->LoadTextureFromTiff("border.tif");
	windowmat->GetTexture()->SetTransparencyRange(25, 75, 25, 75, true);
	
	auto orangemat = std::make_shared<Material>();
	orangemat->SetShininess(4.0f);
	orangemat->LoadTextureFromTiff("orange.tif");
	
	auto bordermat = std::make_shared<Material>();
	bordermat->SetShininess(4.0f);
	bordermat->LoadTextureFromTiff("border.tif");
	
	auto uvmat = std::make_shared<Material>();
	uvmat->LoadTextureFromTiff("uv-test.tif");
	
	auto viewmat = std::make_shared<Material>();
	viewmat->LoadTextureFromTiff("view.tif");

	auto catmat = std::make_shared<Material>();
	catmat->LoadTextureFromTiff("cat.tif");

	auto treemat = std::make_shared<Material>();
	treemat->LoadTextureFromTiff("tree.tif");

	auto gimat = std::make_shared<Material>();
	gimat->LoadTextureFromTiff("GI.tif");

	auto teapot1k = new Model(orangemat, true, true);
	teapot1k->GetMesh().LoadBin("geometry/teapot1K.bin");
	teapot1k->SetCenter(vec3(0.0f, 0.0f, 0.0f));
	teapot1k->SetScale(vec3(1.0, 1.0, 1.0));
	
	auto cube = new Model(windowmat, true, true);
	cube->GetMesh().SetToCube(vec3(0, 0, 0), 5, vec3(128, 0, 128).GetColor(), vec3(128, 128, 0).GetColor());
	cube->SetCenter(vec3(60.0f, 0.0f, 0.0f));
	cube->SetScale(vec3(9.0, 9.0, 9.0));
	
	auto quad2 = new Model(catmat);
	quad2->GetMesh().SetToQuad(vec3(0, 0, 0), 5, vec3(128, 0, 128).GetColor(), vec3(128, 128, 0).GetColor());
	quad2->SetCenter(vec3(0.0f, 60.0f, 0.0f));
	quad2->SetScale(vec3(9.0, 9.0, 9.0));

	auto quad3 = new Model(treemat);
	quad3->GetMesh().SetToQuad(vec3(0, 0, 0), 5, vec3(128, 0, 128).GetColor(), vec3(128, 128, 0).GetColor());
	quad3->SetCenter(vec3(-60.0f, 0.0f, 0.0f));
	quad3->SetScale(vec3(9.0, 9.0, 9.0));

	auto quad4 = new Model(uvmat);
	quad4->GetMesh().SetToQuad(vec3(0, 0, 0), 5, vec3(128, 0, 128).GetColor(), vec3(128, 128, 0).GetColor());
	quad4->SetCenter(vec3(0.0f, -60.0f, 0.0f));
	quad4->SetScale(vec3(9.0, 9.0, 9.0));

	auto quad5 = new Model(gimat);
	quad5->GetMesh().SetToQuad(vec3(0, 0, 0), 5, vec3(128, 0, 128).GetColor(), vec3(128, 128, 0).GetColor());
	quad5->SetCenter(vec3(60.0f, 60.0f, 0.0f));
	quad5->SetScale(vec3(9.0f, 9.0f, 9.0f));


	auto quad6 = new Model(viewmat);
	quad6->GetMesh().SetToQuad(vec3(0, 0, 0), 5, vec3(128, 0, 128).GetColor(), vec3(128, 128, 0).GetColor());
	quad6->SetCenter(vec3(-60.0f, 60.0f, 0.0f));
	quad6->SetScale(vec3(9.0f, 9.0f, 9.0f));

	auto quad7 = new Model(viewmat);
	quad7->GetMesh().SetToQuad(vec3(0, 0, 0), 5, vec3(128, 0, 128).GetColor(), vec3(128, 128, 0).GetColor(), 3.0f);
	quad7->SetCenter(vec3(-60.0f, -60.0f, 0.0f));
	quad7->SetScale(vec3(9.0f, 9.0f, 9.0f));

	_Models.push_back(teapot1k);
	_Models.push_back(cube);
	_Models.push_back(quad2);
	_Models.push_back(quad3);
	_Models.push_back(quad4);
	_Models.push_back(quad5);
	_Models.push_back(quad6);
	_Models.push_back(quad7);
#endif

#ifdef A4_1
	_PointLights.emplace_back();
	_PointLights[0].position = vec3(0, 50, 0);
	_PointLights[0].diffuse = vec3(1.0);
	_PointLights[0].specular = vec3(2.0);
	_PointLights[0].constant = 1.0f;
	_PointLights[0].linear = 0.001f;
	_PointLights[0].quadratic = 0.00075f;
	
	auto bordermat = std::make_shared<Material>();
	bordermat->SetShininess(4.0f);
	bordermat->LoadTextureFromTiff("border.tif");
	
	auto orangemat = std::make_shared<Material>();
	orangemat->SetShininess(4.0f);
	orangemat->LoadTextureFromTiff("orange.tif");
	auto teapotC = new Model(orangemat, true, true);
	teapotC->GetMesh().LoadBin("geometry/teapot1K.bin");
	teapotC->SetCenter(vec3(10.0f, 10.0f, 10.0f));
	teapotC->SetScale(vec3(0.2));

	auto teapotR = new Model(orangemat, true, true);
	teapotR->GetMesh().LoadBin("geometry/teapot1K.bin");
	teapotR->SetCenter(vec3(30.0f, 10.0f, 30.0f));
	teapotR->SetScale(vec3(0.3));
	
	auto cube2 = new Model(orangemat, true, true);
	cube2->GetMesh().SetToCube(vec3(0, 0, 0), 10, vec3(128, 0, 128).GetColor(), vec3(128, 128, 0).GetColor());
	cube2->SetCenter(vec3(15.0f, 10.0f, -15.0f));
	cube2->SetScale(vec3(1.0, 1.0, 1.0));
	auto cube3 = new Model(orangemat, true, true);
	cube3->GetMesh().SetToCube(vec3(0, 0, 0), 10, vec3(128, 0, 128).GetColor(), vec3(128, 128, 0).GetColor());
	cube3->SetCenter(vec3(-15.0f, 10.0f, 15.0f));
	cube3->SetScale(vec3(1.0, 1.0, 1.0));
	
	auto cube4 = new Model(orangemat, true, true);
	cube4->GetMesh().SetToCube(vec3(0, 0, 0), 10, vec3(128, 0, 128).GetColor(), vec3(128, 128, 0).GetColor());
	cube4->SetCenter(vec3(-15.0f, 10.0f, -15.0f));
	cube4->SetScale(vec3(1.0, 1.0, 1.0));
	
	auto quad0 = new Model(bordermat, true, true);
	quad0->GetMesh().SetToQuad(vec3(0, -40, 0), 100, vec3(0.8).GetColor(), vec3(0.8).GetColor());
	
	quad0->SetScale(vec3(1.0, 1.0, 1.0));
	quad0->GetMesh().Rotate(quad0->GetMesh().GetCenter(), vec3(1.0f, 0.0f, 0.0f), -90.0f);
	quad0->SetCenter(vec3(0.0f, 0.0f, 0.0f));
	quad0->SetDefaultFillMode(_FillMode_Vertex_Color_Lighting);

	auto quad1 = new Model(bordermat, true, true);
	quad1->GetMesh().SetToQuad(vec3(0, -40, 0), 100, vec3(0.8).GetColor(), vec3(0.8).GetColor());

	quad1->SetScale(vec3(1.0, 1.0, 1.0));
	quad1->GetMesh().Rotate(quad1->GetMesh().GetCenter(), vec3(0.0f, 1.0f, 0.0f), 90.0f);
	quad1->GetMesh().Scale(vec3(1.0f, 0.4f, 1.0f));
	quad1->SetCenter(vec3(-50.0f, 20.0f, 0.0f));
	quad1->SetDefaultFillMode(_FillMode_Vertex_Color_Lighting);
	
	auto quad2 = new Model(bordermat, true, true);
	quad2->GetMesh().SetToQuad(vec3(0, -40, 0), 100, vec3(0.8).GetColor(), vec3(0.8).GetColor());

	quad2->SetScale(vec3(1.0, 1.0, 1.0));
	quad2->GetMesh().Rotate(quad2->GetMesh().GetCenter(), vec3(0.0f, 1.0f, 0.0f), -90.0f);
	quad2->SetCenter(vec3(50.0f, 20.0f, 0.0f));
	quad2->GetMesh().Scale(vec3(1.0f, 0.4f, 1.0f));
	quad2->SetDefaultFillMode(_FillMode_Vertex_Color_Lighting);
	
	auto quad3 = new Model(bordermat, true, true);
	quad3->GetMesh().SetToQuad(vec3(0, -40, 0), 100, vec3(0.8).GetColor(), vec3(0.8).GetColor());

	quad3->SetScale(vec3(1.0, 1.0, 1.0));
	quad3->GetMesh().Rotate(quad3->GetMesh().GetCenter(), vec3(0.0f, 1.0f, 0.0f), 0.0f);
	quad3->SetCenter(vec3(0.0f, 20.0f, -50.0f));
	quad3->GetMesh().Scale(vec3(1.0f, 0.4f, 1.0f));
	quad3->SetDefaultFillMode(_FillMode_Vertex_Color_Lighting);
	
	auto quad4 = new Model(bordermat, true, true);
	quad4->GetMesh().SetToQuad(vec3(0, -40, 0), 100, vec3(0.8).GetColor(), vec3(0.8).GetColor());

	quad4->SetScale(vec3(1.0, 1.0, 1.0));
	quad4->GetMesh().Rotate(quad4->GetMesh().GetCenter(), vec3(0.0f, 1.0f, 0.0f), 180.0f);
	quad4->SetCenter(vec3(0.0f, 20.0f, 50.0f));
	quad4->GetMesh().Scale(vec3(1.0f, 0.4f, 1.0f));
	quad4->SetDefaultFillMode(_FillMode_Vertex_Color_Lighting);
	
	_Models.push_back(teapotC);
	_Models.push_back(cube2);
	_Models.push_back(cube3);
	_Models.push_back(cube4);
	
	_Models.push_back(quad1);
	_Models.push_back(quad2);
	_Models.push_back(quad3);
	_Models.push_back(quad4);
	_Models.push_back(quad0);
	_Models.push_back(teapotR);
#endif

#ifdef A4_E2
	_PointLights.emplace_back();
	_PointLights[0].position = vec3(20, 50, 0);
	_PointLights[0].diffuse = vec3(0.2);
	_PointLights[0].specular = vec3(0.4);
	_PointLights[0].constant = 1.0f;
	_PointLights[0].linear = 0.001f;
	_PointLights[0].quadratic = 0.00075f;

	_PointLights.emplace_back();
	_PointLights[1].position = vec3(-20, 50, 0);
	_PointLights[1].diffuse = vec3(0.2, 0.0, 0.0);
	_PointLights[1].specular = vec3(0.4, 0.0, 0.0);
	_PointLights[1].constant = 1.0f;
	_PointLights[1].linear = 0.001f;
	_PointLights[1].quadratic = 0.00075f;

	_PointLights.emplace_back();
	_PointLights[2].position = vec3(0, 50, 20);
	_PointLights[2].diffuse = vec3(0.0, 0.2, 0.0);
	_PointLights[2].specular = vec3(0.0, 0.4, 0.0);
	_PointLights[2].constant = 1.0f;
	_PointLights[2].linear = 0.001f;
	_PointLights[2].quadratic = 0.00075f;

	_PointLights.emplace_back();
	_PointLights[3].position = vec3(0, 50, -20);
	_PointLights[3].diffuse = vec3(0.0, 0.0, 0.2);
	_PointLights[3].specular = vec3(0.0, 0.0, 0.4);
	_PointLights[3].constant = 1.0f;
	_PointLights[3].linear = 0.001f;
	_PointLights[3].quadratic = 0.00075f;
	
	auto bordermat = std::make_shared<Material>();
	bordermat->SetShininess(4.0f);
	bordermat->LoadTextureFromTiff("border.tif");

	auto orangemat = std::make_shared<Material>();
	orangemat->SetShininess(4.0f);
	orangemat->LoadTextureFromTiff("orange.tif");
	auto cube1 = new Model(orangemat, true, true);
	cube1->GetMesh().SetToCube(vec3(0, 0, 0), 10, vec3(128, 0, 128).GetColor(), vec3(128, 128, 0).GetColor());
	cube1->SetCenter(vec3(0.0f, 30.0f, 0.0f));
	cube1->SetScale(vec3(1.0, 1.0, 1.0));
	
	auto quad0 = new Model(bordermat, true, true);
	quad0->GetMesh().SetToQuad(vec3(0, -40, 0), 100, vec3(0.8).GetColor(), vec3(0.8).GetColor());

	quad0->SetScale(vec3(1.0, 1.0, 1.0));
	quad0->GetMesh().Rotate(quad0->GetMesh().GetCenter(), vec3(1.0f, 0.0f, 0.0f), -90.0f);
	quad0->SetCenter(vec3(0.0f, 0.0f, 0.0f));
	quad0->SetDefaultFillMode(_FillMode_Vertex_Color_Lighting);

	auto quad1 = new Model(bordermat, true, true);
	quad1->GetMesh().SetToQuad(vec3(0, -40, 0), 100, vec3(0.8).GetColor(), vec3(0.8).GetColor());

	quad1->SetScale(vec3(1.0, 1.0, 1.0));
	quad1->GetMesh().Rotate(quad1->GetMesh().GetCenter(), vec3(0.0f, 1.0f, 0.0f), 90.0f);
	quad1->GetMesh().Scale(vec3(1.0f, 0.4f, 1.0f));
	quad1->SetCenter(vec3(-50.0f, 20.0f, 0.0f));
	quad1->SetDefaultFillMode(_FillMode_Vertex_Color_Lighting);

	auto quad2 = new Model(bordermat, true, true);
	quad2->GetMesh().SetToQuad(vec3(0, -40, 0), 100, vec3(0.8).GetColor(), vec3(0.8).GetColor());

	quad2->SetScale(vec3(1.0, 1.0, 1.0));
	quad2->GetMesh().Rotate(quad2->GetMesh().GetCenter(), vec3(0.0f, 1.0f, 0.0f), -90.0f);
	quad2->SetCenter(vec3(50.0f, 20.0f, 0.0f));
	quad2->GetMesh().Scale(vec3(1.0f, 0.4f, 1.0f));
	quad2->SetDefaultFillMode(_FillMode_Vertex_Color_Lighting);

	auto quad3 = new Model(bordermat, true, true);
	quad3->GetMesh().SetToQuad(vec3(0, -40, 0), 100, vec3(0.8).GetColor(), vec3(0.8).GetColor());

	quad3->SetScale(vec3(1.0, 1.0, 1.0));
	quad3->GetMesh().Rotate(quad3->GetMesh().GetCenter(), vec3(0.0f, 1.0f, 0.0f), 0.0f);
	quad3->SetCenter(vec3(0.0f, 20.0f, -50.0f));
	quad3->GetMesh().Scale(vec3(1.0f, 0.4f, 1.0f));
	quad3->SetDefaultFillMode(_FillMode_Vertex_Color_Lighting);

	auto quad4 = new Model(bordermat, true, true);
	quad4->GetMesh().SetToQuad(vec3(0, -40, 0), 100, vec3(0.8).GetColor(), vec3(0.8).GetColor());

	quad4->SetScale(vec3(1.0, 1.0, 1.0));
	quad4->GetMesh().Rotate(quad4->GetMesh().GetCenter(), vec3(0.0f, 1.0f, 0.0f), 180.0f);
	quad4->SetCenter(vec3(0.0f, 20.0f, 50.0f));
	quad4->GetMesh().Scale(vec3(1.0f, 0.4f, 1.0f));
	quad4->SetDefaultFillMode(_FillMode_Vertex_Color_Lighting);

	_Models.push_back(cube1);
	_Models.push_back(quad1);
	_Models.push_back(quad2);
	_Models.push_back(quad3);
	_Models.push_back(quad4);
	_Models.push_back(quad0);
#endif

#ifdef A4_2
	_ProjLimit = 0;
	
	_ProjBuffer = new FrameBuffer(u0, v0, 300, 300, 1);
	_ProjCamera = new Camera(90, _ProjBuffer->Width, _ProjBuffer->Height);
	_FrameBuffer->label("Proj Depth Map");
	_ProjBuffer->show();
	_ProjBuffer->redraw();
	_ProjTexture = new Texture();
	_ProjDir = vec3(0, -1, 0);
	_ProjTexture->LoadTiff("happy.tif");
	_CameraPos = vec3(0, 1, 15);
	_CameraDir = vec3(0, -0.2f, -1);
	_ProjPos = vec3(0, 0, 0);
	
	_DirectionalLights.emplace_back();
	_DirectionalLights[0].diffuse = 0.2f;
	_DirectionalLights[0].specular = 0.7f;
	_DirectionalLights[0].direction = vec3(0, 0, -1);
	
	auto orangemat = std::make_shared<Material>();
	orangemat->SetShininess(4.0f);
	orangemat->LoadTextureFromTiff("orange.tif");
	auto bordermat = std::make_shared<Material>();
	bordermat->SetShininess(4.0f);
	bordermat->LoadTextureFromTiff("border.tif");
	
	auto audi = new Model(orangemat, true, true);
	audi->GetMesh().LoadBin("geometry/auditorium.bin");
	audi->SetCenter(vec3(0.0f, 0.0f, 0.0f));
	audi->SetScale(vec3(2.0, 2.0, 2.0));
	audi->GetMesh().Rotate(audi->GetMesh().GetCenter(), vec3(1, 0, 0), -90);
	_Models.push_back(audi);

#endif
	MainLoop();	
}

void Scene::Render() const
{
	_FrameBuffer->SetBGR(0xFFFFFFFF);
	_FrameBuffer->ClearZBuffer();
#ifdef A2
	for (const auto& mesh : _Meshes)
	{
		if (!mesh->Enabled)
			continue;
		mesh->DrawFilled(_FrameBuffer, _MainCamera, _FillMode_Vertex_Color);
	}
#endif
#ifdef A3
	for (const auto& model : _Models)
	{
		if (!model->Enabled)
			continue;
		model->Draw(_FrameBuffer, _MainCamera, _FillMode_Texture_Bilinear);
	}
#endif
#ifdef A4_1
	for (auto& pl : _PointLights)
	{
		pl.ShadowMap.Clear();
		for (const auto& model : _Models)
		{
			if (!model->Enabled)
				continue;
			model->GetMesh().CastPointLightShadow(pl);
		}
	}
	
	for (const auto& model : _Models)
	{
		if (!model->Enabled)
			continue;
		model->Draw(_FrameBuffer, _MainCamera);
	}
#endif
#ifdef A4_2
	_ProjBuffer->SetBGR(0xFFFFFFFF);
	_ProjBuffer->ClearZBuffer();

	for (const auto& model : _Models)
	{
		if (!model->Enabled)
			continue;
		model->Draw(_ProjBuffer, _ProjCamera, _FillMode_Z);
	}
	_ProjBuffer->redraw();
	for (const auto& model : _Models)
	{
		if (!model->Enabled)
			continue;
		model->Draw(_FrameBuffer, _MainCamera);
	}
	_FrameBuffer->ProjectImage(_MainCamera, _ProjCamera, _ProjBuffer, _ProjTexture);
#endif
#ifdef A4_E2
	for (auto& pl : _PointLights)
	{
		pl.ShadowMap.Clear();
		for (const auto& model : _Models)
		{
			if (!model->Enabled)
				continue;
			model->GetMesh().CastPointLightShadow(pl);
		}
	}

	for (const auto& model : _Models)
	{
		if (!model->Enabled)
			continue;
		model->Draw(_FrameBuffer, _MainCamera);
	}
#endif
	_FrameBuffer->redraw();
}

void Scene::MainLoop()
{
	float delta = 0.1f;
	auto start = std::chrono::high_resolution_clock::now();
	// A floating point milliseconds type
	using FpSeconds =
		std::chrono::duration<float, std::chrono::seconds::period>;

	static_assert(std::chrono::treat_as_floating_point<FpSeconds::rep>::value,
		"Rep required to be floating point");
	
	while (true)
	{
		Fl::check();
		auto stop = std::chrono::high_resolution_clock::now();
		float lastFrameTime = _CurrentTime;
		_CurrentTime = FpSeconds(stop - start).count();
		//cout << std::to_string(_CurrentTime - lastFrameTime) << endl;
		Update();
		if (_CurrentTime - _LastTimeStep >= 0.033f)
		{
			_LastTimeStep = _CurrentTime;
			FixedUpdate();
		}
		LateUpdate();
		Render();
	}
}

void Scene::FixedUpdate()
{
#ifdef A2
	_Meshes[1]->Rotate(_Meshes[1]->GetCenter(), vec3(1.0f, 1.0f, 0.0f), 2.0f);
	_Meshes[2]->Rotate(_Meshes[2]->GetCenter(), vec3(1.0f, 0.0f, 1.0f), 2.0f);
	_Meshes[4]->Rotate(_Meshes[4]->GetCenter(), vec3(0.0f, 1.0f, 1.0f), 2.0f);
	_Meshes[5]->Rotate(_Meshes[5]->GetCenter(), vec3(0.0f, 0.0f, 1.0f), 2.0f);
#endif
#ifdef A3
	int i = 0;
	for(auto* model : _Models)
	{
		i++;
		if (i % 8 == 0) i++;
		model->GetMesh().Rotate(model->GetMesh().GetCenter(), vec3(i % 8 > 0 ? 1 : 0, i % 4 > 0 ? 1 : 0, i % 2 > 0 ? 1 : 0), 2.0f);
	}	
#endif
#ifdef A4_1
	_Models[0]->GetMesh().Rotate(_Models[0]->GetMesh().GetCenter(), vec3(1.0f, 1.0f, 0.0f), 2.0f);
	_Models[1]->GetMesh().Rotate(_Models[1]->GetMesh().GetCenter(), vec3(1.0f, 0.0f, 0.0f), 2.0f);
	_Models[2]->GetMesh().Rotate(_Models[2]->GetMesh().GetCenter(), vec3(0.0f, 0.0f, 1.0f), 2.0f);
	_Models[3]->GetMesh().Rotate(_Models[3]->GetMesh().GetCenter(), vec3(0.0f, 1.0f, 0.0f), 2.0f);
	_PointLights[0].position = vec3(0.0f, sin(_CurrentTime) * 20.0f + 20.0f, 0.0f);
#endif
#ifdef A4_E2
	const float sway = 27;
	const float height = 50;
	_PointLights[0].position = vec3(sin(_CurrentTime) * sway + sway, height, 0.0f);
	_PointLights[1].position = vec3(sin(_CurrentTime) * sway - sway, height, 0.0f);
	_PointLights[2].position = vec3(0.0f, height, sin(_CurrentTime) * sway + sway);
	_PointLights[3].position = vec3(0.0f, height, sin(_CurrentTime) * sway - sway);
#endif

}

void Scene::Update()
{
#ifdef A2
	if (_CurrentTime > 5.0f && _CurrentTime < 10.0f) {

		vec3 target = vec3(0.0f, 0.0f, 0.0f);
		vec3 center = vec3(200.0f * sin((_CurrentTime - 5.0f) / 5.0f * 3.1415926f), 0.0f, 200.0f * cos((_CurrentTime - 5.0f) / 5.0f * 3.1415926f));
		_MainCamera->SetPose(center, target, vec3(0.0f, 1.0f, 0.0f));	
	}
#endif
#ifdef A3
	_DirectionalLights[0].direction = vec3(sin(_CurrentTime * 5), 0, cos(_CurrentTime * 5));
	vec3 target = vec3(0.0f, 0.0f, 0.0f);
	vec3 center = vec3(200.0f * sin(_CurrentTime / 5.0f * 3.1415926f), 0.0f, 200.0f * cos(_CurrentTime / 5.0f * 3.1415926f));
	_MainCamera->SetPose(center, target, vec3(0.0f, 1.0f, 0.0f));
#endif
#ifdef A4_1
	vec3 target = vec3(0.0f, 0.0f, 0.0f);
	vec3 center = vec3(80.0f * sin(_CurrentTime / 10.0f * 3.1415926f), 120.0f, 80.0f * cos(_CurrentTime / 10.0f * 3.1415926f));
	_MainCamera->SetPose(center, target, vec3(0.0f, 1.0f, 0.0f));
#endif
#ifdef A4_2
	if (Fl::event_key(44)) {
		_ProjLimit -= 5;
		if (_ProjLimit < 0) _ProjLimit = 0;
		_ProjTexture->SetAllTransparency(false);
		_ProjTexture->SetTransparencyRange(_ProjTexture->GetWidth() / 2 - _ProjLimit, _ProjTexture->GetWidth() / 2 + _ProjLimit, _ProjTexture->GetHeight() / 2 - _ProjLimit, _ProjTexture->GetHeight() / 2+ _ProjLimit, true);
	}
	if (Fl::event_key(46)) {
		_ProjLimit += 5;
		if (_ProjLimit > 140) _ProjLimit = 140;
		_ProjTexture->SetAllTransparency(false);
		_ProjTexture->SetTransparencyRange(_ProjTexture->GetWidth() / 2 - _ProjLimit, _ProjTexture->GetWidth() / 2 + _ProjLimit, _ProjTexture->GetHeight() / 2 - _ProjLimit, _ProjTexture->GetHeight() / 2 + _ProjLimit, true);
	}
	if (Fl::event_key(119)) {
		_ProjDir[2] -= 0.2f;
		if (_ProjDir[2] < -5) _ProjDir[2] = -5;
	}
	if (Fl::event_key(115)) {
		_ProjDir[2] += 0.2f;
		if (_ProjDir[2] > 5) _ProjDir[2] = 5;
	}
	if (Fl::event_key(97)) {
		_ProjDir[0] -= 0.2f;
		if (_ProjDir[0] < -5) _ProjDir[0] = -5;
	}
	if (Fl::event_key(100)) {
		_ProjDir[0] += 0.2f;
		if (_ProjDir[0] > 5) _ProjDir[0] = 5;
	}
	if (Fl::event_key(FL_Shift_L)) {
		_CameraPos[1] += 0.2f;
		if (_CameraPos[1] > 5) _CameraPos[1] = 5;
	}
	if (Fl::event_key(FL_Control_L)) {
		_CameraPos[1] -= 0.2f;
		if (_CameraPos[1] < -3) _CameraPos[1] = -3;
	}
	
	if (Fl::event_key(FL_Up)) {
		_ProjPos[2] -= 1.0f;
		if (_ProjPos[2] < -5) _ProjPos[2] = -5;
	}
	if (Fl::event_key(FL_Down)) {
		_ProjPos[2] += 1.0f;
		if (_ProjPos[2] > 5) _ProjPos[2] = 5;
	}
	if (Fl::event_key(FL_Left)) {
		_ProjPos[0] -= 1.0f;
		if (_ProjPos[0] < -15) _ProjPos[0] = -15;
	}
	if (Fl::event_key(FL_Right)) {
		_ProjPos[0] += 1.0f;
		if (_ProjPos[0] > 15) _ProjPos[0] = 15;
	}
	if (Fl::event_key(FL_Shift_R)) {
		_ProjPos[1] += 1.0f;
		if (_ProjPos[1] > 5) _ProjPos[1] = 5;
	}
	if (Fl::event_key(FL_Control_R)) {
		_ProjPos[1] -= 1.0f;
		if (_ProjPos[1] < -3) _ProjPos[1] = -3;
	}
	_ProjCamera->SetPose(_ProjPos, _ProjPos + _ProjDir, vec3(0, 0, 1));
	_MainCamera->SetPose(_CameraPos, _CameraPos + _CameraDir, vec3(0.0f, 1.0f, 0.0f));
#endif
}

void Scene::LateUpdate()
{
#ifdef A2

#endif
#ifdef A3

#endif
}

void Scene::DBG()
{
	_FrameBuffer->SaveAsTiff("a.tif");
	_PointLights[0].ShadowMap.SaveAsTiff("cm");
}
void Scene::NewButton()
{
	cerr << "INFO: pressed New Button" << endl;
	
}