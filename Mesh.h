#pragma once

#include "vec3.h"
#include "Camera.h"
#include "Bounds.h"
#include "framebuffer.h"

class Mesh
{
	Bounds _BoundingBox;
	vec3* _Verts;
	vec3* _Colors;
	vec3* _Normals;
	int _VertsN;
	unsigned int* _Tris;
	int _TrisN;
public:
	bool Enabled;

	Mesh() : _Verts(nullptr), _Colors(nullptr), _Normals(nullptr), _VertsN(0), _Tris(nullptr), _TrisN(0), Enabled(true)
	{
	}

	void RecalculateBoundingBox();
	void SetToCube(vec3 cc, float sideLength, unsigned int color0, unsigned int color1);
	void Allocate(int vertsN, int trisN);
	void DrawCubeQuadFaces(FrameBuffer* fb, Camera* ppc, unsigned int color) const;
	void DrawWireFrame(FrameBuffer* fb, Camera* ppc, unsigned int color) const;
	void DrawFilled(FrameBuffer* fb, Camera* ppc) const;
	void LoadBin(char* filename);
	static vec3 SetEEQs(vec3 v0, vec3 v1, vec3 v2);
	vec3 GetCenter();
	void SetCenter(vec3 center);
	void Translate(vec3 tv);
	void Rotate(vec3 aO, vec3 aDir, float theta);
};
