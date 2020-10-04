#pragma once
#include "Texture.h"
class Material
{
	Texture* _Texture;
public:
	Texture* GetTexture() const { return _Texture; }
	void SetTexture(Texture* texture) { _Texture = texture; }
};

