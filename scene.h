#pragma once


#include "gui.h"
#include "framebuffer.h"
#include "Camera.h"
#include "Mesh.h"
#include "ThreadPool.h"
#include <vector>


#include "Light.h"
#include "Model.h"

class Scene
{
	friend class Mesh;
	static float _AmbientLight;
	static vector<DirectionalLight> _DirectionalLights;
	static vector<PointLight> _PointLights;
	static vec3 _CameraPos;
	static vec3 _CameraDir;
	static Texture* _ProjTexture;
	static vec3 _ProjPos;
	static Camera* _ProjCamera;
	static FrameBuffer* _ProjBuffer;
	static ThreadPool _ThreadPool;
	static vector<Mesh*> _Meshes;
	static vector<Model*> _Models;
	static Camera* _MainCamera;
	static FrameBuffer* _FrameBuffer;
	static GUI* _GUI;
	static vec3 _ProjDir;
	static float _LastTimeStep;
	static float _CurrentTime;
public:
	static ThreadPool* GetThreadPool();
	Scene();
	static void DBG();
	static void NewButton();
	void Render() const;

	void MainLoop();
	
	void FixedUpdate();
	void Update();
	void LateUpdate();
};

extern Scene* scene;
