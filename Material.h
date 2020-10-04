#pragma once
#include <utility>
#include "Texture.h"
class Material
{
	friend class Mesh;
	std::shared_ptr<Texture> _Texture;
	float _Shininess;
public:
	Material() { _Texture = std::make_shared<Texture>(); }
	void SetShininess(float value) { _Shininess = value; }
	std::shared_ptr<Texture> GetTexture() const { return _Texture; }
	void SetTexture(std::shared_ptr<Texture> texture) { _Texture = std::move(texture); }
	void LoadTextureFromTiff(std::string fileName) const { _Texture->LoadTiff(std::move(fileName)); }
	void SaveTextureAsTiff(std::string fileName) const { _Texture->SaveAsTiff(std::move(fileName)); }
};

