#include "stdafx.h"

#include "scene.h"

#include "vec3.h"
#include "mat3.h"
#include "Camera.h"
#include "Mesh.h"

Scene* scene;

ThreadPool Scene::_ThreadPool;

using namespace std;

#include <iostream>

ThreadPool* Scene::GetThreadPool()
{
	return &_ThreadPool;
}

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
	
	Render();
}

void Scene::Render() const
{
	_FrameBuffer->SetBGR(0xFFFFFFFF);
	_FrameBuffer->ClearZBuffer();

	for (const auto& mesh : _Meshes)
	{
		if (!mesh->Enabled)
			continue;
		mesh->DrawFilled(_FrameBuffer, _Camera, _FillMode_Vertex_Color);
	}
	_FrameBuffer->redraw();
}

void Scene::FixedUpdate()
{
	//_Meshes[0]->Rotate(_Meshes[0]->GetCenter(), vec3(0.0f, 1.0f, 0.0f), 2.0f);
	_Meshes[1]->Rotate(_Meshes[1]->GetCenter(), vec3(1.0f, 1.0f, 0.0f), 2.0f);
	_Meshes[2]->Rotate(_Meshes[2]->GetCenter(), vec3(1.0f, 0.0f, 1.0f), 2.0f);
	_Meshes[4]->Rotate(_Meshes[4]->GetCenter(), vec3(0.0f, 1.0f, 1.0f), 2.0f);
	_Meshes[5]->Rotate(_Meshes[5]->GetCenter(), vec3(0.0f, 0.0f, 1.0f), 2.0f);
}

void Scene::Update()
{
	if (currentTime > 5.0f && currentTime < 10.0f) {
		vec3 target = vec3(0.0f ,0.0f, 0.0f);
		vec3 center = vec3(200.0f * sin((currentTime - 5.0f) / 5.0f * 3.1415926f), 0.0f, 200.0f * cos((currentTime - 5.0f) / 5.0f * 3.1415926f));
		_Camera->SetPose(center, target, vec3(0.0f, 1.0f, 0.0f));
	}
}

void Scene::LateUpdate()
{
	
}

void Scene::DBG()
{
	
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
		if(currentTime - lastTimeStep >= 0.01f)
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