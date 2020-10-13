#pragma once

#include <mutex>
#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>
#include "Texture.h"
#include "Camera.h"

class FrameBuffer : public Fl_Gl_Window
{
public:
	vector<unsigned> Pixels; // pixel array
	vector<float> ZBuffer;
	vector<mutex> ZBufferLocks;
	int Width, Height;
	FrameBuffer(int u0, int v0, int width, int height, unsigned int _id);
	void draw() override;
	void KeyboardHandle();
	int handle(int event) override;
	void SetBGR(unsigned int bgr);
	void Set(int u, int v, unsigned int color)
	{
		if (u < 0 || u > Width - 1 || v < 0 || v > Height - 1)
			return;

		Pixels[(Height - 1 - v) * Width + u] = color;
	}
	float GetZ(int u, int v)
	{
		return ZBuffer[(Height - 1 - v) * Width + u];
	}
	void Resize(int width, int height);
	void SetZ(int u, int v, float z, unsigned int color)
	{
		if (u < 0 || v < 0 || u > Width - 1 || v > Height - 1)
			return;

		int uv = (Height - 1 - v) * Width + u;

		std::lock_guard<mutex> lock(ZBufferLocks[uv]);
		if (ZBuffer[uv] > z)
			return;
		ZBuffer[uv] = z;
		Pixels[(Height - 1 - v) * Width + u] = color;
	}
	void LoadTiff(char* fileName);
	void SaveAsTiff(char* fileName);
	void SetChecker(unsigned int col0, unsigned int col1, int csize);
	void Draw3DSegment(vec3 from, vec3 to, Camera* ppc, vec3 c0, vec3 c1);
	void Draw2DSegment(vec3 p0, vec3 c0, vec3 p1, vec3 c1);
	void DrawSquarePoint(float uf, float vf, int psize, unsigned int color);
	bool Farther(int u, int v, float z);
	void ProjectImage(Camera* camera, Camera* projCamera, FrameBuffer* fb, Texture* tex);
	void ClearZBuffer();
};
