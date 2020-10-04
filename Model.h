#pragma once
#include "Mesh.h"
#include "Material.h"
class Model
{
	Mesh* _Mesh;
	Material* _Material;
public:
	bool Enabled;
	Model(Mesh* mesh, Material* material, bool enabled = true)
	{
		Enabled = enabled;
		_Mesh = mesh;
		_Material = material;
	}
	void SetCenter(vec3 value) const
	{
		if (_Mesh != nullptr) _Mesh->SetCenter(value);
	}
	void SetScale(vec3 value) const
	{
		if (_Mesh != nullptr) _Mesh->Scale(value);
	}
	Mesh* GetMesh() const { return _Mesh; }
	Material* GetMaterial() const { return _Material; }
	void Draw(FrameBuffer* fb, Camera* camera, FillMode mode) const
	{
		_Mesh->DrawFilled(fb, camera, mode, _Material);
	}
};

