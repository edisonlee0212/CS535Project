#pragma once

#include <vector>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "Camera.h"

class FrameBuffer : public Fl_Gl_Window
{
public:
	vector<unsigned> Pixels; // pixel array
	vector<float> ZBuffer;
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
	void Resize(int width, int height);
	void SetZ(int u, int v, float z, unsigned int color)
	{
		if (u < 0 || v < 0 || u > Width - 1 || v > Height - 1)
			return;

		int uv = (Height - 1 - v) * Width + u;
		if (ZBuffer[uv] > z)
			return;
		ZBuffer[uv] = z;

		if (u < 0 || u > Width - 1 || v < 0 || v > Height - 1)
			return;

		Pixels[(Height - 1 - v) * Width + u] = color;
	}
	void LoadTiff(char* fileName);
	void SaveAsTiff(char* fileName);
	void SetChecker(unsigned int col0, unsigned int col1, int csize);
	void Draw3DSegment(vec3 from, vec3 to, Camera* ppc, vec3 c0, vec3 c1);
	void Draw2DSegment(vec3 p0, vec3 c0, vec3 p1, vec3 c1);
	void DrawSquarePoint(float uf, float vf, int psize, unsigned int color);
	bool Farther(int u, int v, float z);
	void ClearZBuffer();
};
