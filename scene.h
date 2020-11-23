#pragma once

#include "framebuffer.h"
#include "Camera.h"
#include "Mesh.h"
#include "ThreadPool.h"
#include <vector>


#include <glfw3.h>
#include "Cubemap.h"
#include "Light.h"
#include "Model.h"

class Scene
{
public:
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
	static vec3 _ProjDir;
	static int _ProjLimit;
	static float _RollAngle;
	static float _VerticalAngle;
	static float _HorizontalAngle;
	static float _Distance;
	static float _LastTimeStep;
	static float _CurrentTime;
	static float _DeltaTime;
	static bool _EnableTextureMapping;
	static bool _EnableWireFrameMode;
	static Cubemap _Skybox;
	static GLFWwindow* _Window;

	static ThreadPool* GetThreadPool();
	static void RenderSkybox();
	static void MainLoop();
	static void RenderHW();
	static void FixedUpdate();
	static void Update();
	static void LateUpdate();
};

