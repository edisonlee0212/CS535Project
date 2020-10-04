#pragma once
#include <utility>
#include "Texture.h"
class Material
{
	std::shared_ptr<Texture> _Texture;
public:
	Material() { _Texture = std::make_shared<Texture>(); }
	std::shared_ptr<Texture> GetTexture() const { return _Texture; }
	void SetTexture(std::shared_ptr<Texture> texture) { _Texture = std::move(texture); }
	void LoadTextureFromTiff(std::string fileName) const { _Texture->LoadTiff(std::move(fileName)); }
	void SaveTextureAsTiff(std::string fileName) const { _Texture->SaveAsTiff(std::move(fileName)); }
};

