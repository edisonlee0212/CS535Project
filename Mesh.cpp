#include "stdafx.h"

#include <fstream>
#include <iostream>

#include "Mesh.h"


#include <future>
#include <vector>

#include "scene.h"
#include "mat3.h"
#include "Material.h"

using namespace std;

void Mesh::Allocate(int vertsN, int trisN)
{
	_VertsN = vertsN;
	_TrisN = trisN;
	_Verts.resize(_VertsN);
	_Colors.resize(_VertsN);
	_Normals.resize(_VertsN);
	_TexCoords.resize(_VertsN);
	_Tris.resize(3 * _TrisN);
}

void Mesh::RecalculateBoundingBox()
{
	_BoundingBox.MaxBound = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	_BoundingBox.MinBound = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	std::vector<shared_future<void>> results;
	const auto tSize = Scene::GetThreadPool()->Size();
	size_t group = _VertsN / tSize;
	results.reserve(tSize);
	for (auto t = 0; t < tSize; t++)
	{
		results.push_back(Scene::GetThreadPool()->Push([t, group, this](int id)
			{
				for (auto i = t * group; i < (t + 1) * group; i++)
				{
					if (_BoundingBox.MaxBound[0] < _Verts[i][0]) _BoundingBox.MaxBound[0] = _Verts[i][0];
					if (_BoundingBox.MaxBound[1] < _Verts[i][1]) _BoundingBox.MaxBound[1] = _Verts[i][1];
					if (_BoundingBox.MaxBound[2] < _Verts[i][2]) _BoundingBox.MaxBound[2] = _Verts[i][2];
					if (_BoundingBox.MinBound[0] > _Verts[i][0]) _BoundingBox.MinBound[0] = _Verts[i][0];
					if (_BoundingBox.MinBound[1] > _Verts[i][1]) _BoundingBox.MinBound[1] = _Verts[i][1];
					if (_BoundingBox.MinBound[2] > _Verts[i][2]) _BoundingBox.MinBound[2] = _Verts[i][2];
				}
			}).share());
	}
	for (auto i = tSize * group; i < _VertsN; i++)
	{
		if (_BoundingBox.MaxBound[0] < _Verts[i][0]) _BoundingBox.MaxBound[0] = _Verts[i][0];
		if (_BoundingBox.MaxBound[1] < _Verts[i][1]) _BoundingBox.MaxBound[1] = _Verts[i][1];
		if (_BoundingBox.MaxBound[2] < _Verts[i][2]) _BoundingBox.MaxBound[2] = _Verts[i][2];
		if (_BoundingBox.MinBound[0] > _Verts[i][0]) _BoundingBox.MinBound[0] = _Verts[i][0];
		if (_BoundingBox.MinBound[1] > _Verts[i][1]) _BoundingBox.MinBound[1] = _Verts[i][1];
		if (_BoundingBox.MinBound[2] > _Verts[i][2]) _BoundingBox.MinBound[2] = _Verts[i][2];
	}
	for (size_t i = 0; i < results.size(); i++) {
		results[i].wait();
	}
}

void Mesh::RecalculateNormals()
{
	std::vector<std::vector<vec3>> normalLists;
	normalLists.resize(_VertsN);
	for (size_t i = 0; i < _TrisN; i++) {
		auto v1 = _Verts.at(_Tris.at(3 * i));
		auto v2 = _Verts.at(_Tris.at(3 * i + 1));
		auto v3 = _Verts.at(_Tris.at(3 * i + 2));
		auto normal = ((v1 - v2) ^ (v1 - v3)).Normalized();
		normalLists[_Tris.at(3 * i)].push_back(normal);
		normalLists[_Tris.at(3 * i + 1)].push_back(normal);
		normalLists[_Tris.at(3 * i + 2)].push_back(normal);
	}
	_Normals.resize(_VertsN);
	for (auto i = 0; i < _VertsN; i++) {
		vec3 normal = vec3(0.0f, 0.0f, 0.0f);
		for (auto j : normalLists[i]) {
			normal = normal + j;
		}
		_Normals[i] = normal.Normalized();
	}
	HasNormals = true;
}

void Mesh::SetToCube(vec3 cc, float sideLength, unsigned int color0, unsigned int color1)
{
	_VertsN = 24;
	_TrisN = 6 * 2;
	Allocate(_VertsN, _TrisN);

	HasTexCoords = true;
	HasColors = true;
	HasNormals = false;
	int vi = 0;
	//1234
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 1);
	vi++;
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 0);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 0);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 1);
	vi++;

	//5678
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, +sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 1);
	vi++;
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, -sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 0);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, -sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 0);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, +sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 1);
	vi++;

	//1256
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 0);
	vi++;
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 0);
	vi++;
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, +sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 1);
	vi++;
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, -sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 1);
	vi++;

	//3478
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 0);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 0);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, -sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 1);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, +sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 1);
	vi++;

	//2367
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 0);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 0);
	vi++;
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, -sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 1);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, -sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 1);
	vi++;

	//1458
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 0);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 0);
	vi++;
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, +sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 1);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, +sideLength / 2.0f, -sideLength / 2.0f);
	_TexCoords[vi] = vec2(1, 1);
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
	_Tris[3 * tri + 0] = 6;
	_Tris[3 * tri + 1] = 5;
	_Tris[3 * tri + 2] = 4;
	tri++;
	_Tris[3 * tri + 0] = 7;
	_Tris[3 * tri + 1] = 6;
	_Tris[3 * tri + 2] = 4;
	tri++;
	_Tris[3 * tri + 0] = 8;
	_Tris[3 * tri + 1] = 10;
	_Tris[3 * tri + 2] = 9;
	tri++;
	_Tris[3 * tri + 0] = 10;
	_Tris[3 * tri + 1] = 11;
	_Tris[3 * tri + 2] = 9;
	tri++;
	_Tris[3 * tri + 0] = 13;
	_Tris[3 * tri + 1] = 12;
	_Tris[3 * tri + 2] = 15;
	tri++;
	_Tris[3 * tri + 0] = 14;
	_Tris[3 * tri + 1] = 15;
	_Tris[3 * tri + 2] = 12;
	tri++;
	_Tris[3 * tri + 0] = 19;
	_Tris[3 * tri + 1] = 17;
	_Tris[3 * tri + 2] = 16;
	tri++;
	_Tris[3 * tri + 0] = 16;
	_Tris[3 * tri + 1] = 18;
	_Tris[3 * tri + 2] = 19;
	tri++;
	_Tris[3 * tri + 0] = 20;
	_Tris[3 * tri + 1] = 21;
	_Tris[3 * tri + 2] = 23;
	tri++;
	_Tris[3 * tri + 0] = 20;
	_Tris[3 * tri + 1] = 23;
	_Tris[3 * tri + 2] = 22;
	tri++;
	RecalculateBoundingBox();
	RecalculateNormals();


}

void Mesh::SetToQuad(vec3 cc, float sideLength, unsigned color0, unsigned color1, float tiledFactor)
{
	_VertsN = 4;
	_TrisN = 2;
	Allocate(_VertsN, _TrisN);

	HasTexCoords = true;
	HasColors = true;
	HasNormals = false;
	int vi = 0;
	//1234
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, 0);
	vi++;
	_Verts[vi] = cc + vec3(-sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(0, tiledFactor);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(tiledFactor, tiledFactor);
	vi++;
	_Verts[vi] = cc + vec3(+sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	_TexCoords[vi] = vec2(tiledFactor, 0);
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
	RecalculateNormals();
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

void Mesh::DrawFilled(FrameBuffer* fb, Camera* camera, FillMode mode, Material* material, bool receiveLight) const
{
	vector<vec3> proj;
	proj.resize(_VertsN);
	std::vector<shared_future<void>> results;
	auto tSize = Scene::GetThreadPool()->Size();
	size_t group = _VertsN / tSize;
	results.reserve(tSize);
#pragma region Points projection
	for (auto t = 0; t < tSize; t++)
	{
		results.push_back(Scene::GetThreadPool()->Push([t, group, this, &camera, &proj](int id)
			{
				for (auto i = t * group; i < (t + 1) * group; i++)
				{
					camera->Project(_Verts[i], proj[i]);
				}
			}).share());
	}
	for (auto i = tSize * group; i < _VertsN; i++)
	{
		camera->Project(_Verts[i], proj[i]);
	}
	for (auto& result : results)
	{
		result.wait();
	}
#pragma endregion

#pragma region SetColor
	results.clear();
	tSize = Scene::GetThreadPool()->Size();
	group = _TrisN / tSize;
	results.reserve(tSize);
	std::mutex writeMutex;
	/*for(auto i = 0; i < _TrisN; i++)
	{
		results.push_back(Scene::GetThreadPool()->Push([i, this, &proj, &fb, &writeMutex, mode, &camera, &material](int id)
		{
			RasterizationHelper(i, fb, proj, camera, mode, writeMutex, material);
		}).share());
	}
	*/

	for (auto t = 0; t < tSize; t++)
	{
		results.push_back(Scene::GetThreadPool()->Push([t, group, this, &proj, &fb, &writeMutex, mode, &camera, &material, receiveLight](int id)
			{
				for (auto i = t * group; i < (t + 1) * group; i++)
				{
					RasterizationHelper(i, fb, proj, camera, mode, writeMutex, material, receiveLight);
				}
			}).share());
	}

	results.push_back(Scene::GetThreadPool()->Push([tSize, group, this, &proj, &fb, &writeMutex, mode, &camera, &material, receiveLight](int id) {
		for (auto i = tSize * group; i < _TrisN; i++)
		{
			RasterizationHelper(i, fb, proj, camera, mode, writeMutex, material, receiveLight);
		}
		}).share());


	for (auto& result : results)
	{
		result.wait();
	}
#pragma endregion
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
	HasColors = false;
	HasNormals = false;
	HasTexCoords = false;

	_Verts.resize(_VertsN);

	ifs.read(&yn, 1); // cols 3 floats

	if (yn == 'y')
	{
		HasColors = true;
		_Colors.resize(_VertsN);
	}

	ifs.read(&yn, 1); // normals 3 floats

	if (yn == 'y')
	{
		HasNormals = true;
		_Normals.resize(_VertsN);
	}

	ifs.read(&yn, 1); // texture coordinates 2 floats

	if (yn == 'y')
	{
		HasTexCoords = true;
		_TexCoords.resize(_VertsN);
	}

	ifs.read((char*)_Verts.data(), 3 * _VertsN * sizeof(float)); // load verts

	if (HasColors)
	{
		ifs.read((char*)_Colors.data(), 3 * _VertsN * sizeof(float)); // load cols
	}

	if (HasNormals)
		ifs.read((char*)_Normals.data(), 3 * _VertsN * sizeof(float)); // load normals

	if (HasTexCoords)
		ifs.read((char*)_TexCoords.data(), 2 * _VertsN * sizeof(float)); // load texture coordinates

	ifs.read((char*)&_TrisN, sizeof(int));

	_Tris.resize(3 * _TrisN);
	ifs.read((char*)_Tris.data(), 3 * _TrisN * sizeof(unsigned int)); // read triangles

	ifs.close();

	cerr << "INFO: loaded " << _VertsN << " verts, " << _TrisN << " tris from " << endl << "      " << filename << endl;
	cerr << "      value " << (HasColors ? "rgb " : "") << (HasNormals ? "nxnynz " : "") << (HasTexCoords ? "tcstct " : "") <<
		endl;
	RecalculateBoundingBox();
	RecalculateNormals();
}

vec3 Mesh::GetCenter()
{
	return _BoundingBox.GetCenter();
}

void Mesh::Translate(vec3 value)
{
	std::vector<shared_future<void>> results;
	const auto tSize = Scene::GetThreadPool()->Size();
	size_t group = _VertsN / tSize;
	results.reserve(tSize);
	for (auto t = 0; t < tSize; t++)
	{
		results.push_back(Scene::GetThreadPool()->Push([t, group, this, value](int id)
			{
				for (auto i = t * group; i < (t + 1) * group; i++)
				{
					_Verts[i] = _Verts[i] + value;
				}
			}).share());
	}
	for (auto i = tSize * group; i < _VertsN; i++)
	{
		_Verts[i] = _Verts[i] + value;
	}
	for (size_t i = 0; i < results.size(); i++) {
		results[i].wait();
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
	std::vector<shared_future<void>> results;
	const auto tSize = Scene::GetThreadPool()->Size();
	size_t group = _VertsN / tSize;
	results.reserve(tSize);
	for (auto t = 0; t < tSize; t++)
	{
		results.push_back(Scene::GetThreadPool()->Push([t, group, this, aO, aDir, theta](int id)
			{
				for (auto i = t * group; i < (t + 1) * group; i++)
				{
					_Verts[i] = _Verts[i].RotatePoint(aO, aDir, theta);
					_Normals[i] = _Normals[i].RotateVector(aDir, theta);
				}
			}).share());
	}
	for (auto i = tSize * group; i < _VertsN; i++)
	{
		_Verts[i] = _Verts[i].RotatePoint(aO, aDir, theta);
		_Normals[i] = _Normals[i].RotateVector(aDir, theta);
	}
	for (size_t i = 0; i < results.size(); i++) {
		results[i].wait();
	}
	RecalculateBoundingBox();
}

void Mesh::Scale(vec3 value)
{
	vec3 center = GetCenter();
	SetCenter(vec3(0.0f, 0.0f, 0.0f));

	std::vector<shared_future<void>> results;
	const auto tSize = Scene::GetThreadPool()->Size();
	size_t group = _VertsN / tSize;
	results.reserve(tSize);
	for (auto t = 0; t < tSize; t++)
	{
		results.push_back(Scene::GetThreadPool()->Push([t, group, this, &value](int id)
			{
				for (auto i = t * group; i < (t + 1) * group; i++)
				{
					_Verts[i][0] *= value[0];
					_Verts[i][1] *= value[1];
					_Verts[i][2] *= value[2];
				}
			}).share());
	}
	for (auto i = tSize * group; i < _VertsN; i++)
	{
		_Verts[i][0] *= value[0];
		_Verts[i][1] *= value[1];
		_Verts[i][2] *= value[2];
	}
	for (size_t i = 0; i < results.size(); i++) {
		results[i].wait();
	}
	SetCenter(center);
}

inline void Mesh::RasterizationHelper(int i, FrameBuffer* fb, vector<vec3>& proj, Camera* camera, FillMode mode, std::mutex& writeMutex, Material* material, bool calculateLighting) const
{
	vec3 projectedVertices[3], vertices[3], colors[3], normals[3], texCoords[3];
	projectedVertices[0] = proj[_Tris[3 * i + 0]];
	projectedVertices[1] = proj[_Tris[3 * i + 1]];
	projectedVertices[2] = proj[_Tris[3 * i + 2]];
	vertices[0] = _Verts[_Tris[3 * i + 0]];
	vertices[1] = _Verts[_Tris[3 * i + 1]];
	vertices[2] = _Verts[_Tris[3 * i + 2]];
	colors[0] = _Colors[_Tris[3 * i + 0]];
	colors[1] = _Colors[_Tris[3 * i + 1]];
	colors[2] = _Colors[_Tris[3 * i + 2]];
	normals[0] = _Normals[_Tris[3 * i + 0]];
	normals[1] = _Normals[_Tris[3 * i + 1]];
	normals[2] = _Normals[_Tris[3 * i + 2]];
	if (HasTexCoords)
	{
		texCoords[0] = _TexCoords[_Tris[3 * i + 0]];
		texCoords[1] = _TexCoords[_Tris[3 * i + 1]];
		texCoords[2] = _TexCoords[_Tris[3 * i + 2]];
	}
	else
	{
		texCoords[0] = vec3(0, 0, 0);
		texCoords[1] = vec3(0, 0, 0);
		texCoords[2] = vec3(0, 0, 0);
	}
	if (projectedVertices[0][0] == FLT_MAX || projectedVertices[1][0] == FLT_MAX || projectedVertices[2][0] == FLT_MAX) return;
	Bounds bound;
	bound.MaxBound = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	bound.MinBound = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	for (auto& pv : projectedVertices)
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
		eeqs[vi][0] = projectedVertices[(vi + 1) % 3][1] - projectedVertices[vi][1];
		eeqs[vi][1] = projectedVertices[vi][0] - projectedVertices[(vi + 1) % 3][0];
		eeqs[vi][2] = -projectedVertices[vi][0] * eeqs[vi][0] + projectedVertices[vi][1] * -eeqs[vi][1];
		vec3 v2p(projectedVertices[(vi + 2) % 3][0], projectedVertices[(vi + 2) % 3][1], 1.0f);
		if (v2p * eeqs[vi] < 0.0f)
			eeqs[vi] = eeqs[vi] * -1.0f;
	}
	if (!bound.Clip(minClip, maxClip, 2))
		return;
	const auto left = static_cast<int>(bound.MinBound[0] + 0.5f);
	const auto right = static_cast<int>(bound.MaxBound[0] - 0.5f);
	const auto top = static_cast<int>(bound.MinBound[1] + 0.5f);
	const auto bottom = static_cast<int>(bound.MaxBound[1] - 0.5f);


	mat3 screenSpaceInterpolationMat;
	screenSpaceInterpolationMat.SetColumn(0, vec3(projectedVertices[0][0], projectedVertices[1][0], projectedVertices[2][0]));
	screenSpaceInterpolationMat.SetColumn(1, vec3(projectedVertices[0][1], projectedVertices[1][1], projectedVertices[2][1]));
	screenSpaceInterpolationMat.SetColumn(2, vec3(1.0f, 1.0f, 1.0f));
	screenSpaceInterpolationMat = screenSpaceInterpolationMat.Inverted();



	mat3 vs;
	vs[0] = vertices[0];
	vs[1] = vertices[1];
	vs[2] = vertices[2];
	mat3 cs;
	cs[0] = colors[0];
	cs[1] = colors[1];
	cs[2] = colors[2];
	mat3 nms;
	nms[0] = normals[0];
	nms[1] = normals[1];
	nms[2] = normals[2];
	mat3 ttcs;
	ttcs[0] = texCoords[0];
	ttcs[1] = texCoords[1];
	ttcs[2] = texCoords[2];

	mat3 modelSpaceInterpolationMat = GetModelSpaceInterpolationMat(vs, camera);
	vec3 densityABC = modelSpaceInterpolationMat[0] + modelSpaceInterpolationMat[1] + modelSpaceInterpolationMat[2];
	mat3 vertexPosABC = vs.Transpose() * modelSpaceInterpolationMat;
	mat3 colorsABC = cs.Transpose() * modelSpaceInterpolationMat;
	mat3 normalsABC = nms.Transpose() * modelSpaceInterpolationMat;
	mat3 texCoordABC = ttcs.Transpose() * modelSpaceInterpolationMat;

	vec3 z;
	vec3 r, g, b;
	if (mode == _FillMode_Vertex_Color_ScreenSpaceInterpolation)
	{
		r = screenSpaceInterpolationMat * vec3(colors[0][0], colors[1][0], colors[2][0]);
		g = screenSpaceInterpolationMat * vec3(colors[0][1], colors[1][1], colors[2][1]);
		b = screenSpaceInterpolationMat * vec3(colors[0][2], colors[1][2], colors[2][2]);
	}

	z = screenSpaceInterpolationMat * vec3(projectedVertices[0][2], projectedVertices[1][2], projectedVertices[2][2]);

	vec3 viewPos = camera->Center;

	switch (mode)
	{
	case _FillMode_Z:
		for (auto v = top; v <= bottom; v++)
		{
			for (auto u = left; u <= right; u++)
			{
				vec3 pix(.5f + static_cast<float>(u), .5f + static_cast<float>(v), 1.0f);
				if (pix * eeqs[0] < 0.0f || pix * eeqs[1] < 0.0f
					|| pix * eeqs[2] < 0.0f)
					continue;
				vec3 ccv(pix * z, pix * z, pix * z);
				pix[2] = pix * z;
				std::lock_guard<std::mutex> lock(writeMutex);
				if (!fb->Farther(pix[0], pix[1], pix[2]))
				{
					continue;
				}
				fb->Set(pix[0], pix[1], ccv.GetColor());
			}
		}
		break;
	case _FillMode_Vertex_Color_ModelSpaceInterpolation:
		for (auto v = top; v <= bottom; v++)
		{
			for (auto u = left; u <= right; u++)
			{
				vec3 pix(.5f + static_cast<float>(u), .5f + static_cast<float>(v), 1.0f);
				if (pix * eeqs[0] < 0.0f || pix * eeqs[1] < 0.0f
					|| pix * eeqs[2] < 0.0f)
					continue;
				vec3 ccv = (colorsABC * pix) / (densityABC * pix);
				pix[2] = pix * z;
				std::lock_guard<std::mutex> lock(writeMutex);
				if (!fb->Farther(pix[0], pix[1], pix[2]))
				{
					continue;
				}
				fb->Set(pix[0], pix[1], ccv.GetColor());
			}
		}
		break;
	case _FillMode_Vertex_Color_ScreenSpaceInterpolation:
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
				std::lock_guard<std::mutex> lock(writeMutex);
				if (!fb->Farther(pix[0], pix[1], pix[2]))
				{
					continue;
				}
				fb->Set(pix[0], pix[1], ccv.GetColor());
			}
		}
		break;
	case _FillMode_Texture_Nearest:
	case _FillMode_Texture_Bilinear:
	case _FillMode_Texture_Trilinear:
		for (auto v = top; v <= bottom; v++)
		{
			for (auto u = left; u <= right; u++)
			{
				vec3 pixel(.5f + static_cast<float>(u), .5f + static_cast<float>(v), 1.0f);
				if (pixel * eeqs[0] < 0.0f || pixel * eeqs[1] < 0.0f
					|| pixel * eeqs[2] < 0.0f)
					continue;
				vec3 texCoord = (texCoordABC * pixel) / (densityABC * pixel);
				vec3 normal = (normalsABC * pixel) / (densityABC * pixel);

				/*vec3 normal(pixel * (ssim * vec3(normals[0][0], normals[1][0], normals[2][0])),
					pixel * (ssim * vec3(normals[0][1], normals[1][1], normals[2][1])),
					pixel * (ssim * vec3(normals[0][2], normals[1][2], normals[2][2])));

				vec3 texCoord(pixel * (ssim * vec3(texCoords[0][0], texCoords[1][0], texCoords[2][0])),
					pixel * (ssim * vec3(texCoords[0][1], texCoords[1][1], texCoords[2][1])),
					pixel * (ssim * vec3(texCoords[0][2], texCoords[1][2], texCoords[2][2])));
				*/
				pixel[2] = pixel * z;
				std::lock_guard<std::mutex> lock(writeMutex);
				if (material != nullptr && material->GetTexture() != nullptr) {
					if (material->GetTexture()->IsTransparent(texCoord[0], texCoord[1])) continue;
					if (!fb->Farther(pixel[0], pixel[1], pixel[2])) continue;
					unsigned surfaceColor = 0;
					switch (mode)
					{
					case _FillMode_Texture_Nearest:
						surfaceColor = material->GetTexture()->Nearest(texCoord[0], texCoord[1]);
						break;
					case _FillMode_Texture_Bilinear:
						surfaceColor = material->GetTexture()->Bilinear(texCoord[0], texCoord[1]);
						break;
					case _FillMode_Texture_Trilinear:
						surfaceColor = material->GetTexture()->Trilinear(texCoord[0], texCoord[1], pixel[2]);
						break;
					}

					if (calculateLighting)
					{
						vec3 surfaceColV3;
						surfaceColV3.SetFromColor(surfaceColor);
						vec3 fragPos = ((vertexPosABC * pixel) / (densityABC * pixel)).Normalized();
						vec3 viewDir = (viewPos - fragPos).Normalized();

						vec3 result = vec3(Scene::_AmbientLight);
						for (auto& dl : Scene::_DirectionalLights)
						{
							vec3 lightDir = (vec3(0, 0, 0) - dl.direction).Normalized();
							vec3 diffuse = dl.diffuse * max(normal * lightDir, 0.0f);
							vec3 reflectDir = (vec3(0, 0, 0) - lightDir).Reflect(normal);
							vec3 specular = dl.specular * pow(max((viewDir * reflectDir), 0.0f), material->_Shininess);
							result = result + diffuse + specular;
						}
						surfaceColor = surfaceColV3.Multiply(result).GetColor();

					}
					fb->SetZ(pixel[0], pixel[1], pixel[2], surfaceColor);
				}
				else
				{
					fb->Set(pixel[0], pixel[1], vec3(0.5, 0, 0.5).GetColor());
				}
			}
		}
		break;
	}
}
