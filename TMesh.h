#pragma once

#include "V3.h"
#include "ppc.h"
#include "aabb.h"
#include "framebuffer.h"

class TMesh {
public:
	AABB BoundingBox;
	int OnFlag;
	V3 *Verts;
	V3 *Colors;
	V3 *Normals;
	int VertsN;
	unsigned int *Tris;
	int TrisN;
	TMesh() : OnFlag(1), Verts(nullptr), Colors(nullptr), Normals(nullptr), VertsN(0), Tris(nullptr), TrisN(0) {}
	void SetToCube(V3 cc, float sideLength, unsigned int color0, unsigned int color1);
	void Allocate(int vertsN, int trisN);
	void DrawCubeQuadFaces(FrameBuffer *fb, PPC *ppc, unsigned int color);
	void DrawWireFrame(FrameBuffer *fb, PPC *ppc, unsigned int color);
	void LoadBin(char *filename);
	V3 GetCenter();
	void SetCenter(V3 center);
	void Translate(V3 tv);
	void Rotate(V3 aO, V3 aDir, float theta);
};