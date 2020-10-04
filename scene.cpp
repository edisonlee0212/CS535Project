#include "stdafx.h"

#include "scene.h"

#include "vec3.h"
#include "mat3.h"
#include "Camera.h"
#include "Mesh.h"

Scene* scene;

ThreadPool Scene::_ThreadPool;
vector<DirectionalLight> Scene::_DirectionalLights;
vector<PointLight> Scene::_PointLights;
const float Scene::_AmbientLight = 0.1f;
using namespace std;

#include <iostream>

ThreadPool* Scene::GetThreadPool()
{
	return &_ThreadPool;
}
#define A3
Scene::Scene()
{
	gui = new GUI();
	gui->show();
	_ThreadPool.Resize(std::thread::hardware_concurrency());

	int u0 = 20;
	int v0 = 20;
	int h = 720;
	int w = 1280;

	_FrameBuffer = new FrameBuffer(u0, v0, w, h, 0);
	_FrameBuffer->label("SW frame buffer");
	_FrameBuffer->show();
	_FrameBuffer->redraw();

	gui->uiw->position(u0, v0 + h + 50);
	float hfov = 75.0f;
	_Camera = new Camera(hfov, _FrameBuffer->Width, _FrameBuffer->Height);

	_Camera->SetPose(vec3(0.0f, 0.0f, 200.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	_DirectionalLights.emplace_back();
	_DirectionalLights[0].diffuse = 0.2f;
	_DirectionalLights[0].specular = 0.7f;
	_DirectionalLights[0].direction = vec3(0, 0, -1);
	
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

	
	Render();
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
		mesh->DrawFilled(_FrameBuffer, _Camera, _FillMode_Vertex_Color_ScreenSpaceInterpolation);
	}
#endif
#ifdef A3
	for (const auto& model : _Models)
	{
		if (!model->Enabled)
			continue;
		model->Draw(_FrameBuffer, _Camera, _FillMode_Texture_Bilinear);
	}
#endif
	_FrameBuffer->redraw();
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
}

void Scene::Update()
{
#ifdef A2
	if (currentTime > 5.0f && currentTime < 10.0f) {

		vec3 target = vec3(0.0f, 0.0f, 0.0f);
		vec3 center = vec3(200.0f * sin((currentTime - 5.0f) / 5.0f * 3.1415926f), 0.0f, 200.0f * cos((currentTime - 5.0f) / 5.0f * 3.1415926f));
		_Camera->SetPose(center, target, vec3(0.0f, 1.0f, 0.0f));	
	}
#endif
#ifdef A3
	_DirectionalLights[0].direction = vec3(sin(currentTime * 5), 0, cos(currentTime * 5));
	vec3 target = vec3(0.0f, 0.0f, 0.0f);
	vec3 center = vec3(200.0f * sin(currentTime / 5.0f * 3.1415926f), 0.0f, 200.0f * cos(currentTime / 5.0f * 3.1415926f));
	_Camera->SetPose(center, target, vec3(0.0f, 1.0f, 0.0f));
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
	float delta = 0.1f;
	auto start = std::chrono::high_resolution_clock::now();
	// A floating point milliseconds type
	using FpSeconds =
		std::chrono::duration<float, std::chrono::seconds::period>;

	static_assert(std::chrono::treat_as_floating_point<FpSeconds::rep>::value,
		"Rep required to be floating point");
	
	while(true)
	{
		Fl::check();
		auto stop = std::chrono::high_resolution_clock::now();
		currentTime = FpSeconds(stop - start).count();
		Update();
		if(currentTime - lastTimeStep >= 0.033f)
		{
			lastTimeStep = currentTime;
			FixedUpdate();
		}
		LateUpdate();
		Render();
	}
}
void Scene::NewButton()
{
	cerr << "INFO: pressed New Button" << endl;
	
}