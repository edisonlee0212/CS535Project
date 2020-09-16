#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "Camera.h"
#include "Mesh.h"
#include <vector>
class Scene
{
public:
	vector<Mesh*> TMeshes;
	//Mesh* TMeshes;
	Camera* ppc;
	GUI* gui;
	FrameBuffer* fb;
	Scene();
	void DBG() const;
	static void NewButton();
	void Render() const;
};

extern Scene* scene;
