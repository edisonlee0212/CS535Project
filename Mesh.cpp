#include "stdafx.h"

#include <fstream>
#include <iostream>

#include "Mesh.h"

#include <vector>


#include "mat3.h"

using namespace std;


void Mesh::Allocate(int vertsN, int trisN)
{
	_VertsN = vertsN;
	_TrisN = trisN;
	_Verts = new vec3[_VertsN];
	_Colors = new vec3[_VertsN];
	_Normals = new vec3[_VertsN];
	_Tris = new unsigned int[3 * _TrisN];
}

void Mesh::RecalculateBoundingBox()
{
	_BoundingBox.MaxBound = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	_BoundingBox.MinBound = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	for (auto i = 0; i < _VertsN; i++)
	{
		if (_BoundingBox.MaxBound[0] < _Verts[i][0]) _BoundingBox.MaxBound[0] = _Verts[i][0];
		if (_BoundingBox.MaxBound[1] < _Verts[i][1]) _BoundingBox.MaxBound[1] = _Verts[i][1];
		if (_BoundingBox.MaxBound[2] < _Verts[i][2]) _BoundingBox.MaxBound[2] = _Verts[i][2];
		if (_BoundingBox.MinBound[0] > _Verts[i][0]) _BoundingBox.MinBound[0] = _Verts[i][0];
		if (_BoundingBox.MinBound[1] > _Verts[i][1]) _BoundingBox.MinBound[1] = _Verts[i][1];
		if (_BoundingBox.MinBound[2] > _Verts[i][2]) _BoundingBox.MinBound[2] = _Verts[i][2];
	}
}

void Mesh::SetToCube(vec3 cc, float sideLength, unsigned int color0, unsigned int color1)
{
	_VertsN = 8;
	_TrisN = 6 * 2;
	Allocate(_VertsN, _TrisN);

	for (int vi = 0; vi < 4; vi++)
	{
		_Colors[vi].SetFromColor(color0);
		_Colors[vi + 4].SetFromColor(color1);
	}

	int vi = 0;
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	vi++;

	_Verts[vi] = _Verts[vi - 4] + vec3(0.0f, 0.0f, -sideLength);
	vi++;
	_Verts[vi] = _Verts[vi - 4] + vec3(0.0f, 0.0f, -sideLength);
	vi++;
	_Verts[vi] = _Verts[vi - 4] + vec3(0.0f, 0.0f, -sideLength);
	vi++;
	_Verts[vi] = _Verts[vi - 4] + vec3(0.0f, 0.0f, -sideLength);
	vi++;

	int tri = 0;
	_Tris[3 * tri + 0] = 0;
	_Tris[3 * tri + 1] = 1;
	_Tris[3 * tri + 2] = 2;
	tri++;
	_Tris[3 * tri + 0] = 2;
	_Tris[3 * tri + 1] = 3;
	_Tris[3 * tri + 2] = 0;
	tri++;
	_Tris[3 * tri + 0] = 3;
	_Tris[3 * tri + 1] = 2;
	_Tris[3 * tri + 2] = 6;
	tri++;
	_Tris[3 * tri + 0] = 6;
	_Tris[3 * tri + 1] = 7;
	_Tris[3 * tri + 2] = 3;
	tri++;
	_Tris[3 * tri + 0] = 7;
	_Tris[3 * tri + 1] = 6;
	_Tris[3 * tri + 2] = 5;
	tri++;
	_Tris[3 * tri + 0] = 5;
	_Tris[3 * tri + 1] = 4;
	_Tris[3 * tri + 2] = 7;
	tri++;
	_Tris[3 * tri + 0] = 4;
	_Tris[3 * tri + 1] = 5;
	_Tris[3 * tri + 2] = 1;
	tri++;
	_Tris[3 * tri + 0] = 1;
	_Tris[3 * tri + 1] = 0;
	_Tris[3 * tri + 2] = 4;
	tri++;
	_Tris[3 * tri + 0] = 4;
	_Tris[3 * tri + 1] = 0;
	_Tris[3 * tri + 2] = 3;
	tri++;
	_Tris[3 * tri + 0] = 3;
	_Tris[3 * tri + 1] = 7;
	_Tris[3 * tri + 2] = 4;
	tri++;
	_Tris[3 * tri + 0] = 1;
	_Tris[3 * tri + 1] = 5;
	_Tris[3 * tri + 2] = 6;
	tri++;
	_Tris[3 * tri + 0] = 6;
	_Tris[3 * tri + 1] = 2;
	_Tris[3 * tri + 2] = 1;
	tri++;
	RecalculateBoundingBox();
}

void Mesh::DrawCubeQuadFaces(FrameBuffer* fb, Camera* ppc, unsigned int color) const
{
	vec3 c0;
	c0.SetFromColor(color);
	for (int si = 0; si < 4; si++)
	{
		fb->Draw3DSegment(_Verts[si], _Verts[(si + 1) % 4], ppc, c0, c0);
		fb->Draw3DSegment(_Verts[4 + si], _Verts[4 + (si + 1) % 4], ppc, c0, c0);
		fb->Draw3DSegment(_Verts[si], _Verts[si + 4], ppc, c0, c0);
	}
}

void Mesh::DrawWireFrame(FrameBuffer* fb, Camera* ppc, unsigned int color) const
{
	for (int tri = 0; tri < _TrisN; tri++)
	{
		vec3 V0 = _Verts[_Tris[3 * tri + 0]];
		vec3 V1 = _Verts[_Tris[3 * tri + 1]];
		vec3 V2 = _Verts[_Tris[3 * tri + 2]];
		vec3 c0 = _Colors[_Tris[3 * tri + 0]];
		vec3 c1 = _Colors[_Tris[3 * tri + 1]];
		vec3 c2 = _Colors[_Tris[3 * tri + 2]];
		fb->Draw3DSegment(V0, V1, ppc, c0, c1);
		fb->Draw3DSegment(V1, V2, ppc, c1, c2);
		fb->Draw3DSegment(V2, V0, ppc, c2, c0);
	}
}

vec3 Mesh::SetEEQs(vec3 v0, vec3 v1, vec3 v2)
{
	vec3 ret;
	ret[0] = v1[1] - v0[1];
	ret[1] = v0[0] - v1[0];
	ret[2] = -v0[0] * ret[0] + v0[1] * -ret[1];

	vec3 v2p(v2[0], v2[1], 1.0f);
	if (v2p * ret < 0.0f)
		ret = ret * -1.0f;

	return ret;
}


void Mesh::DrawFilled(FrameBuffer* fb, Camera* ppc) const
{
	vector<vec3> proj;
	proj.resize(_VertsN);
	for (auto vi = 0; vi < _VertsN; vi++)
	{
		ppc->Project(_Verts[vi], proj[vi]);
	}

	for (int i = 0; i < _TrisN; i++)
	{
		vec3 vertices[3], colors[3];
		vertices[0] = proj[_Tris[3 * i + 0]];
		vertices[1] = proj[_Tris[3 * i + 1]];
		vertices[2] = proj[_Tris[3 * i + 2]];
		colors[0] = _Colors[_Tris[3 * i + 0]];
		colors[1] = _Colors[_Tris[3 * i + 1]];
		colors[2] = _Colors[_Tris[3 * i + 2]];

		if (vertices[0][0] == FLT_MAX || vertices[1][0] == FLT_MAX || vertices[2][0] == FLT_MAX) continue;
		Bounds bound;
		bound.MaxBound = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		bound.MinBound = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		for (auto& pv : vertices)
		{
			if (bound.MaxBound[0] < pv[0]) bound.MaxBound[0] = pv[0];
			if (bound.MaxBound[1] < pv[1]) bound.MaxBound[1] = pv[1];
			if (bound.MaxBound[2] < pv[2]) bound.MaxBound[2] = pv[2];
			if (bound.MinBound[0] > pv[0]) bound.MinBound[0] = pv[0];
			if (bound.MinBound[1] > pv[1]) bound.MinBound[1] = pv[1];
			if (bound.MinBound[2] > pv[2]) bound.MinBound[2] = pv[2];
		}
		vec3 minClip(0.0f, 0.0f, 0.0f);
		vec3 maxClip(static_cast<float>(fb->Width), static_cast<float>(fb->Height), 0.0f);
		vec3 eeqs[3];
		for (auto vi = 0; vi < 3; vi++)
		{
			eeqs[vi][0] = vertices[(vi + 1) % 3][1] - vertices[vi][1];
			eeqs[vi][1] = vertices[vi][0] - vertices[(vi + 1) % 3][0];
			eeqs[vi][2] = -vertices[vi][0] * eeqs[vi][0] + vertices[vi][1] * -eeqs[vi][1];
			vec3 v2p(vertices[(vi + 2) % 3][0], vertices[(vi + 2) % 3][1], 1.0f);
			if (v2p * eeqs[vi] < 0.0f)
				eeqs[vi] = eeqs[vi] * -1.0f;
		}
		if (!bound.Clip(minClip, maxClip, 2))
			continue;
		const auto left = static_cast<int>(bound.MinBound[0] + 0.5f);
		const auto right = static_cast<int>(bound.MaxBound[0] - 0.5f);
		const auto top = static_cast<int>(bound.MinBound[1] + 0.5f);
		const auto bottom = static_cast<int>(bound.MaxBound[1] - 0.5f);

		mat3 mat;
		mat.SetColumn(0, vec3(vertices[0][0], vertices[1][0], vertices[2][0]));
		mat.SetColumn(1, vec3(vertices[0][1], vertices[1][1], vertices[2][1]));
		mat.SetColumn(2, vec3(1.0f, 1.0f, 1.0f));
		mat = mat.Inverted();
		const auto r = mat * vec3(colors[0][0], colors[1][0], colors[2][0]);
		const auto g = mat * vec3(colors[0][1], colors[1][1], colors[2][1]);
		const auto b = mat * vec3(colors[0][2], colors[1][2], colors[2][2]);
		const auto z = mat * vec3(vertices[0][2], vertices[1][2], vertices[2][2]);

		for (auto v = top; v <= bottom; v++)
		{
			for (auto u = left; u <= right; u++)
			{
				vec3 pix(.5f + static_cast<float>(u), .5f + static_cast<float>(v), 1.0f);
				if (pix * eeqs[0] < 0.0f || pix * eeqs[1] < 0.0f
					|| pix * eeqs[2] < 0.0f)
					continue;
				vec3 ccv(pix * r, pix * g, pix * b);
				pix[2] = pix * z;
				fb->SetZ(pix[0], pix[1], pix[2], ccv.GetColor());
			}
		}
	}
}

void Mesh::LoadBin(char* filename)
{
	ifstream ifs(filename, ios::binary);
	if (ifs.fail())
	{
		cerr << "INFO: cannot open file: " << filename << endl;
		return;
	}

	ifs.read((char*)&_VertsN, sizeof(int));
	char yn;
	ifs.read(&yn, 1); // always value
	if (yn != 'y')
	{
		cerr << "INTERNAL ERROR: there should always be vertex value data" << endl;
		return;
	}
	if (_Verts)
		delete _Verts;
	_Verts = new vec3[_VertsN];

	ifs.read(&yn, 1); // cols 3 floats
	if (_Colors)
		delete _Colors;
	_Colors = nullptr;
	if (yn == 'y')
	{
		_Colors = new vec3[_VertsN];
	}

	ifs.read(&yn, 1); // normals 3 floats
	if (_Normals)
		delete _Normals;
	_Normals = nullptr;
	if (yn == 'y')
	{
		_Normals = new vec3[_VertsN];
	}

	ifs.read(&yn, 1); // texture coordinates 2 floats
	float* tcs = nullptr; // don't have texture coordinates for now
	if (tcs)
		delete tcs;
	tcs = nullptr;
	if (yn == 'y')
	{
		tcs = new float[2 * _VertsN];
	}

	ifs.read((char*)_Verts, 3 * _VertsN * sizeof(float)); // load verts

	if (_Colors)
	{
		ifs.read((char*)_Colors, 3 * _VertsN * sizeof(float)); // load cols
	}

	if (_Normals)
		ifs.read((char*)_Normals, 3 * _VertsN * sizeof(float)); // load normals

	if (tcs)
		ifs.read((char*)tcs, 2 * _VertsN * sizeof(float)); // load texture coordinates

	ifs.read((char*)&_TrisN, sizeof(int));
	if (_Tris)
		delete _Tris;
	_Tris = new unsigned int[3 * _TrisN];
	ifs.read((char*)_Tris, 3 * _TrisN * sizeof(unsigned int)); // read tiangles

	ifs.close();

	cerr << "INFO: loaded " << _VertsN << " verts, " << _TrisN << " tris from " << endl << "      " << filename << endl;
	cerr << "      value " << ((_Colors) ? "rgb " : "") << ((_Normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") <<
		endl;
	RecalculateBoundingBox();
}

vec3 Mesh::GetCenter()
{
	return _BoundingBox.GetCenter();
}

void Mesh::Translate(vec3 tv)
{
	for (int vi = 0; vi < _VertsN; vi++)
	{
		_Verts[vi] = _Verts[vi] + tv;
	}
	RecalculateBoundingBox();
}

void Mesh::SetCenter(vec3 center)
{
	vec3 currCenter = GetCenter();
	Translate(center - currCenter);
}


void Mesh::Rotate(vec3 aO, vec3 aDir, float theta)
{
	for (int vi = 0; vi < _VertsN; vi++)
	{
		_Verts[vi] = _Verts[vi].RotatePoint(aO, aDir, theta);
	}
	RecalculateBoundingBox();
}
