#pragma once
#include "gui.h"
#include "framebuffer.h"
#include "Camera.h"
#include "Mesh.h"
#include "ThreadPool.h"
#include <vector>

#include "Model.h"

class Scene
{
	static ThreadPool _ThreadPool;
	vector<Mesh*> _Meshes;
	vector<Model*> _Models;
	Camera* _Camera;
	FrameBuffer* _FrameBuffer;
	GUI* gui;
	float lastTimeStep = 0;
	float currentTime = 0;
public:
	static ThreadPool* GetThreadPool();
	Scene();
	void DBG();
	static void NewButton();
	void Render() const;

	void FixedUpdate();
	void Update();
	void LateUpdate();
};

extern Scene* scene;
