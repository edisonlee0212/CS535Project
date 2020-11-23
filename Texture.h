#pragma once
#include <iostream>
#include <tiffio.h>
#include <vector>
#include "glext.h"
#include "vec3.h"

class Texture
{
	vector<unsigned> _Pixels; // pixel array
	vector<bool> _Transparency;
	int _Width, _Height;
	
public:
	bool Loaded = false;
	GLuint _ID = 0;
	bool NeedUpload = false;
	GLuint ID() const { return _ID; }
	int GetWidth() const
	{
		return _Width;
	}
	int GetHeight() const
	{
		return _Height;
	}
	Texture()
	{
		Resize(0, 0);
	}
	unsigned Get(int u, int v)
	{
		return _Pixels[(_Height - v - 1) * _Width + u];
	}
	void SetAllTransparency(bool value)
	{
		for (auto& i : _Transparency) i = value;
	}
	void SetTransparency(int u, int v, bool value)
	{
		_Transparency[(_Height - v - 1) * _Width + u] = value;
	}
	void SetTransparencyRange(int u1, int u2, int v1, int v2, bool value)
	{
		for(int u = u1; u < u2; u++)
		{
			for(int v = v1; v < v2; v++)
			{
				_Transparency[(_Height - v - 1) * _Width + u] = value;
			}
		}
	}
	Texture(int width, int height)
	{
		Resize(width, height);
	}
	void Resize(int width, int height)
	{
		_Width = width;
		_Height = height;
		_Pixels.resize(_Width * _Height);
		_Transparency.resize(_Width * _Height);
	}
	void LoadTiff(std::string fileName);
	void SaveAsTiff(std::string fileName) const
	{
		TIFF* out = TIFFOpen(fileName.c_str(), "w");

		if (out == nullptr)
		{
			cerr << fileName << " could not be opened" << endl;
			return;
		}

		TIFFSetField(out, TIFFTAG_IMAGEWIDTH, _Width);
		TIFFSetField(out, TIFFTAG_IMAGELENGTH, _Height);
		TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
		TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
		TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

		for (uint32 row = 0; row < static_cast<unsigned>(_Height); row++)
		{
			TIFFWriteScanline(out, (void*)&_Pixels[(_Height - row - 1) * _Width], row);
		}

		TIFFClose(out);
	}
	unsigned Nearest(float x, float y)
	{
		int nearX = static_cast<int>(x * _Width);
		int nearY = static_cast<int>(y * _Height);
		while (nearX > _Width - 1) nearX -= _Width;
		while (nearX < 0) nearX += _Width;
		while (nearY > _Height - 1) nearY -= _Height;
		while (nearY < 0) nearY += _Height;

		return Get(nearX, nearY);
	}
	unsigned Bilinear(float x, float y)
	{
		float nearX = _Width * x;
		float nearY = _Height * y;

		if (nearY < 0.5f) nearY = 0.5f;
		int u = static_cast<int>(nearX - 0.5f);
		int v = static_cast<int>(nearY - 0.5f);
		float du = nearX - (static_cast<float>(u) + 0.5f);
		float dv = nearY - (static_cast<float>(v) + 0.5f);
		u = u % (_Width - 1);
		v = v % ( _Height - 1);
		vec3 color00, color10, color11, color01;
		if (u < 0) u = 0;
		if (v < 0) v = 0;
		if (u > _Width - 2) u = _Width - 2;
		if (v > _Width - 2) v = _Width - 2;

		color00.SetFromColor(Get(u, v));
		color10.SetFromColor(Get(u + 1, v));
		color11.SetFromColor(Get(u + 1, v + 1));
		color01.SetFromColor(Get(u, v + 1));

		vec3 ret;
		ret = color00 * (1.0f - du) * (1.0f - dv)
		+ color10 * du * (1.0f - dv)
		+ color11 * du * dv
		+ color01 * (1.0f - du) * dv;

		return ret.GetColor();
	}
	unsigned Trilinear(float x, float y, float z)
	{
		return 0.0f;
	}	
	bool IsTransparent(float x, float y)
	{
		int nearX = static_cast<int>(x * _Width);
		int nearY = static_cast<int>(y * _Height);
		while (nearX > _Width - 1) nearX -= _Width;
		while (nearX < 0) nearX += _Width;
		while (nearY > _Height - 1) nearY -= _Height;
		while (nearY < 0) nearY += _Height;

		return _Transparency[(_Height - nearY - 1) * _Width + nearX];
	}
	bool IsTransparentI(int x, int y)
	{
		return _Transparency[(_Height - y - 1) * _Width + x];
	}
};

