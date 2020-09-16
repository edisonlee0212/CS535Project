#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "ppc.h"

class FrameBuffer : public Fl_Gl_Window {
public:
	unsigned int *pix; // pixel array
	float *zb;
	int w, h;
	FrameBuffer(int u0, int v0, int _w, int _h, unsigned int _id);
	void draw();
	void KeyboardHandle();
	int handle(int guievent);
	void SetBGR(unsigned int bgr);
	void Set(int u, int v, unsigned int color);
	void LoadTiff(char* fname);
	void SaveAsTiff(char *fname);
	void SetChecker(unsigned int col0, unsigned int col1, int csize);
	void Draw3DSegment(V3 P0, V3 P1, PPC *ppc, V3 c0, V3 c1);
	void Draw2DSegment(V3 p0, V3 c0, V3 p1, V3 c1);
	void DrawSquarePoint(float uf, float vf, int psize, unsigned int color);
	int Farther(int u, int v, float currz);
	void ClearZB();
};