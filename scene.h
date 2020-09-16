#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "TMesh.h"
class Scene {
public:
	TMesh* tmeshes;
	PPC* ppc;
	GUI *gui;
	FrameBuffer *fb;
	Scene();
	size_t tmeshesN;
	void DBG();
	void NewButton();
	void Render();
};

extern Scene *scene;