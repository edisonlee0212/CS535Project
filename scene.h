#pragma once
#include "gui.h"
#include "framebuffer.h"
#include "Camera.h"
#include "Mesh.h"
#include "ThreadPool.h"
#include <vector>
class Scene
{
	static ThreadPool _ThreadPool;
	vector<Mesh*> _Meshes;
	Camera* _Camera;
	FrameBuffer* _FrameBuffer;
	GUI* gui;
public:
	static ThreadPool* GetThreadPool();
	Scene();
	void DBG() const;
	static void NewButton();
	void Render() const;
};

extern Scene* scene;
