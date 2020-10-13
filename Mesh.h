#pragma once

#include <vector>
#include <mutex>
#include "vec3.h"
#include "mat3.h"
#include "Camera.h"
#include "Bounds.h"
#include "framebuffer.h"
#include "Light.h"
class Material;

enum FillMode
{
	_FillMode_Vertex_Color_ScreenSpaceInterpolation,
	_FillMode_Vertex_Color_ModelSpaceInterpolation,
	_FillMode_Z,
	_FillMode_Texture_Nearest,
	_FillMode_Texture_Bilinear,
	_FillMode_Texture_Trilinear,
	_FillMode_Vertex_Color_Lighting
};

class Mesh
{
	Bounds _BoundingBox;
	vector<vec3> _Verts;
	vector<vec3> _Colors;
	vector<vec3> _Normals;
	vector<vec2> _TexCoords;
	int _VertsN;
	vector<unsigned> _Tris;
	int _TrisN;

	inline void PointLightShadowHelper(PointLight& pl, Camera& camera, int index);
	inline void ShadowMapRasterizationHelper(int i, PointLight& pl, vector<vec3>& proj, Camera& camera, std::mutex& writeMutex, int cubeMapIndex);
	inline void RasterizationHelper(int i, FrameBuffer* fb, vector<vec3>& proj, Camera* camera, FillMode mode, std::mutex& writeMutex, Material* material, bool calculateLighting) const;
public:
	bool Enabled;
	bool HasColors;
	bool HasNormals;
	bool HasTexCoords;
	Mesh() : _VertsN(0), _TrisN(0), Enabled(true)
	{
	}

	void RecalculateBoundingBox();
	void RecalculateNormals();
	void SetToCube(vec3 cc, float sideLength, unsigned int color0, unsigned int color1);
	void SetToQuad(vec3 cc, float sideLength, unsigned int color0, unsigned int color1, float tiledFactor = 1.0f);
	void Allocate(int vertsN, int trisN);
	void DrawCubeQuadFaces(FrameBuffer* fb, Camera* ppc, unsigned int color) const;
	void DrawWireFrame(FrameBuffer* fb, Camera* ppc, unsigned int color) const;
	void DrawFilled(FrameBuffer* fb, Camera* camera, FillMode mode = _FillMode_Vertex_Color_ScreenSpaceInterpolation, Material* material = nullptr, bool receiveLight = false) const;
	void CastPointLightShadow(PointLight& pl);
	void LoadBin(char* filename);
	static vec3 SetEEQs(vec3 v0, vec3 v1, vec3 v2);
	vec3 GetCenter();
	void SetCenter(vec3 center);
	void Translate(vec3 value);
	void Rotate(vec3 aO, vec3 aDir, float theta);
	void Scale(vec3 value);
	static mat3 GetModelSpaceInterpolationMat(mat3 vs, Camera* camera)
	{
		mat3 qm = vs.Transpose();
		qm.SetColumn(0, qm.GetColumn(0) - camera->Center);
		qm.SetColumn(1, qm.GetColumn(1) - camera->Center);
		qm.SetColumn(2, qm.GetColumn(2) - camera->Center);

		mat3 cam;
		cam.SetColumn(0, camera->Left);
		cam.SetColumn(1, camera->Up);
		cam.SetColumn(2, camera->c);

		qm = qm.Inverted() * cam;

		return qm;
	}
};
