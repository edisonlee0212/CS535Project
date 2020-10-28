#pragma once
#include "Mesh.h"
#include "Material.h"
class Model
{
	FillMode _DefaultFillMode;
	Mesh _Mesh;
	bool _ReceiveLight;
	std::shared_ptr<Material> _Material;
public:
	bool Enabled;
	Model(std::shared_ptr<Material> material, bool enabled = true, bool receiveLight = false)
	{
		Enabled = enabled;
		_Material = material;
		_ReceiveLight = receiveLight;
		_DefaultFillMode = _FillMode_Texture_Bilinear;
	}
	void SetCenter(vec3 value)
	{
		_Mesh.SetCenter(value);
	}
	void SetScale(vec3 value)
	{
		_Mesh.Scale(value);
	}

	void SetDefaultFillMode(FillMode mode)
	{
		_DefaultFillMode = mode;
	}
	
	Mesh& GetMesh() { return _Mesh; }
	std::shared_ptr<Material> GetMaterial() const { return _Material; }
	void Draw(FrameBuffer* fb, Camera* camera, FillMode mode) const
	{
		_Mesh.DrawFilled(fb, camera, mode, _Material.get(), _ReceiveLight);
	}

	void Draw(FrameBuffer* fb, Camera* camera) const
	{
		_Mesh.DrawFilled(fb, camera, _DefaultFillMode, _Material.get(), _ReceiveLight);
	}
};

