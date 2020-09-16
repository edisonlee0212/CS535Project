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

	_Meshes.push_back(new Mesh());
	_Meshes.push_back(new Mesh());

	_Meshes[0]->LoadBin("geometry/bunny.bin");
	_Meshes[0]->SetCenter(vec3(0.0f, 0.0f, -300.0f));
	_Meshes[0]->Scale(vec3(100.0f, 100.0f, 100.0f));
	_Meshes[1]->LoadBin("geometry/teapot1K.bin");
	_Meshes[1]->SetCenter(vec3(50.0f, 0.0f, -200.0f));

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
		//_Meshes[tmi].DrawWireFrame(_FrameBuffer, _Camera, 0xFF000000);
		mesh->DrawFilled(_FrameBuffer, _Camera);
	}
	_FrameBuffer->redraw();
}

void Scene::DBG() const
{

	vec3 target = _Meshes[0]->GetCenter();
	vec3 center = vec3(20.0f, 50.0f, -30.0f);

	_Meshes[0]->Rotate(_Meshes[0]->GetCenter(), vec3(0, 1, 0), 90);
	_Meshes[1]->Scale(vec3(2.0f, 2.0f, 2.0f));
	_Camera->SetPose(center, target, vec3(0.0f, 1.0f, 0.0f));
	Render();
	return;

}
void Scene::NewButton()
{
	cerr << "INFO: pressed New Button" << endl;
}