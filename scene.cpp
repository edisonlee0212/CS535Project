#include "stdafx.h"
#include "scene.h"
#include <string>
#include <iostream>
#include "vec3.h"
#include "mat3.h"
#include "Camera.h"
#include "Mesh.h"
#define A6
#pragma region Static var decl
float  Scene::_RollAngle = 0;
float  Scene::_VerticalAngle = 0;
float  Scene::_HorizontalAngle = 0;
float  Scene::_Distance = 50.0f;

float Scene::_DeltaTime = 0;
Cubemap Scene::_Skybox;
float Scene::_AmbientLight = 0.1f;
vector<DirectionalLight> Scene::_DirectionalLights;
vector<PointLight> Scene::_PointLights;

ThreadPool Scene::_ThreadPool;
vector<Mesh*> Scene::_Meshes;
vector<Model*> Scene::_Models;
Camera* Scene::_MainCamera;
FrameBuffer* Scene::_FrameBuffer;
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
bool Scene::_EnableWireFrameMode = false;
bool Scene::_EnableTextureMapping = true;
GLFWwindow* Scene::_Window;
#pragma endregion
using namespace std;

void main()
{
	int windowWidth = 1280;
	int windowHeight = 720;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	Scene::_Window = glfwCreateWindow(windowWidth, windowHeight, "Assignment 6", NULL, NULL);
	if (!Scene::_Window) {
		std::cout << "Window Creation Failed" << std::endl;
	}
	glfwMakeContextCurrent(Scene::_Window);
	if (glewInit() != GLEW_OK)
	{
		std::cout << "GLEW initialization failed.\n";
	}
	
	Scene::_ThreadPool.Resize(std::thread::hardware_concurrency());
	const int windowPosX = 20;
	const int windowPosY = 20;
	
	float fov = 60.0f;
	Scene::_FrameBuffer = new FrameBuffer(windowPosX, windowPosY, windowWidth, windowHeight, 0);
	Scene::_MainCamera = new Camera(fov, Scene::_FrameBuffer->Width, Scene::_FrameBuffer->Height);
	Scene::_MainCamera->SetPose(vec3(20.0f, 80.0f, 80.0f), vec3(0), vec3(0.0f, 1.0f, 0.0f));
	Scene::_FrameBuffer->EnableGPURendering = true;
	auto windowmat = std::make_shared<Material>();
	windowmat->SetShininess(4.0f);
	windowmat->LoadTextureFromTiff("Resources/border.tif");
	windowmat->GetTexture()->SetTransparencyRange(25, 75, 25, 75, true);

	auto orangemat = std::make_shared<Material>();
	orangemat->SetShininess(4.0f);
	orangemat->LoadTextureFromTiff("Resources/orange.tif");

	auto bordermat = std::make_shared<Material>();
	bordermat->SetShininess(4.0f);
	bordermat->LoadTextureFromTiff("Resources/border.tif");

	auto uvmat = std::make_shared<Material>();
	uvmat->LoadTextureFromTiff("Resources/uv-test.tif");

	auto viewmat = std::make_shared<Material>();
	viewmat->LoadTextureFromTiff("Resources/view.tif");

	auto catmat = std::make_shared<Material>();
	catmat->LoadTextureFromTiff("Resources/cat.tif");

	auto treemat = std::make_shared<Material>();
	treemat->LoadTextureFromTiff("Resources/tree.tif");

	auto gimat = std::make_shared<Material>();
	gimat->LoadTextureFromTiff("Resources/GI.tif");

	auto teapot1k = new Model(orangemat, true, true);
	teapot1k->GetMesh().LoadBin("Resources/geometry/teapot1K.bin");
	teapot1k->SetCenter(vec3(0.0f, 0.0f, 0.0f));
	teapot1k->SetScale(vec3(1.0, 1.0, 1.0));

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

	Scene::_Models.push_back(teapot1k);
	Scene::_Models.push_back(quad2);
	Scene::_Models.push_back(quad3);
	Scene::_Models.push_back(quad4);
	Scene::_Models.push_back(quad5);
	Scene::_Models.push_back(quad6);
	Scene::_Models.push_back(quad7);

	Scene::MainLoop();
}

void Scene::RenderSkybox()
{
	int resolutionX = _MainCamera->Width;
	int resolutionY = _MainCamera->Height;
	std::vector<shared_future<void>> results;
	for (int i = 0; i < resolutionX; i++)
	{
		results.push_back(_ThreadPool.Push([resolutionY, i](int id)
		{
				for (int j = 0; j < resolutionY; j++)
				{
					vec3 proj = vec3(i, j, 1.0f);
					vec3 pos = _MainCamera->UnProject(proj);
					vec3 dir = pos - _MainCamera->Center;
					unsigned color = _Skybox.GetBilinear(dir);
					_FrameBuffer->SetZ(i, j, 0, color);
				}
		}).share());
	}
	for (auto& result : results)
	{
		result.wait();
	}
}

void Scene::MainLoop()
{
	auto start = std::chrono::high_resolution_clock::now();
	using FpSeconds =
		std::chrono::duration<float, std::chrono::seconds::period>;
	static_assert(std::chrono::treat_as_floating_point<FpSeconds::rep>::value,
		"Rep required to be floating point");

	while (!glfwWindowShouldClose(_Window))
	{
		glfwPollEvents();
		auto stop = std::chrono::high_resolution_clock::now();
		float lastFrameTime = _CurrentTime;
		_CurrentTime = FpSeconds(stop - start).count();
		_DeltaTime = _CurrentTime - lastFrameTime;
		Update();
		if (_CurrentTime - _LastTimeStep >= 0.033f)
		{
			_LastTimeStep = _CurrentTime;
			FixedUpdate();
		}
		LateUpdate();

		RenderHW();

		glfwSwapBuffers(_Window);
	}
}

void Scene::RenderHW()
{
	// initializations (could be done once per session)
	glEnable(GL_DEPTH_TEST);
	if(_EnableWireFrameMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// clear buffers
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	// set the view desired by the application (the user)
	_MainCamera->SetIntrinsicsHW();
	_MainCamera->SetExtrinsicsHW();

	// draw the actual geometry
	for(auto* model : _Models)
	{
		model->DrawHW(_EnableTextureMapping);
	}
}

void Scene::FixedUpdate()
{
}

void Scene::Update()
{
	int i = 0;
	for (auto* model : _Models)
	{
		i++;
		if (i % 8 == 0) i++;
		model->GetMesh().Rotate(model->GetMesh().GetCenter(), vec3(i % 8 > 0 ? 1 : 0, i % 4 > 0 ? 1 : 0, i % 2 > 0 ? 1 : 0), _DeltaTime * 10.0f);
	}
	
	vec3 target = vec3(0.0f, 0.0f, 0.0f);
	vec3 center = vec3(200.0f * sin(_CurrentTime / 5.0f * 3.1415926f), 0.0f, 200.0f * cos(_CurrentTime / 5.0f * 3.1415926f));
	_MainCamera->SetPose(center, target, vec3(0.0f, 1.0f, 0.0f));
	if (glfwGetKey(_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		_EnableWireFrameMode = true;
	}else if(glfwGetKey(_Window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		_EnableWireFrameMode = false;
	}
}

void Scene::LateUpdate()
{

}

ThreadPool* Scene::GetThreadPool()
{
	return &_ThreadPool;
}
