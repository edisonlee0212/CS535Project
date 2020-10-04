#pragma once
#include "Mesh.h"
#include "Material.h"
class Model
{
	Mesh _Mesh;
	std::shared_ptr<Material> _Material;
public:
	bool Enabled;
	Model(std::shared_ptr<Material> material, bool enabled = true)
	{
		Enabled = enabled;
		_Material = material;
	}
	void SetCenter(vec3 value)
	{
		_Mesh.SetCenter(value);
	}
	void SetScale(vec3 value)
	{
		_Mesh.Scale(value);
	}
	Mesh& GetMesh() { return _Mesh; }
	std::shared_ptr<Material> GetMaterial() const { return _Material; }
	void Draw(FrameBuffer* fb, Camera* camera, FillMode mode)
	{
		_Mesh.DrawFilled(fb, camera, mode, _Material.get());
	}
};

