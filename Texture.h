#pragma once
#include <iostream>
#include <tiffio.h>
#include <vector>


#include "vec3.h"

class Texture
{
	vector<unsigned> _Pixels; // pixel array
	int _Width, _Height;
public:
	Texture()
	{
		Resize(0, 0);
	}

	unsigned Get(int u, int v)
	{
		return _Pixels[(_Height - v - 1) * _Width + u];
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
	}
	void LoadTiff(char* fileName)
	{
		TIFF* in = TIFFOpen(fileName, "r");
		if (in == nullptr)
		{
			cerr << fileName << " could not be opened" << endl;
			return;
		}

		int width, height;
		TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
		TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
		if (_Width != width || _Height != height)
		{
			Resize(width, height);
		}

		if (TIFFReadRGBAImage(in, _Width, _Height, _Pixels.data(), 0) == 0)
		{
			cerr << "failed to load " << fileName << endl;
		}

		TIFFClose(in);
	}
	void SaveAsTiff(char* fileName) const
	{
		TIFF* out = TIFFOpen(fileName, "w");

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
		if (nearX > _Width - 0.5f) nearX = static_cast<float>(_Width) - 0.51f;
		if (nearX < 0.5f) nearX = 0.51f;
		if (nearY > _Height - 0.5f) nearY = static_cast<float>(_Height) - 0.51f;
		if (nearY < 0.5f) nearY = 0.51f;
		int u = static_cast<int>(nearX - 0.5f);
		int v = static_cast<int>(nearY - 0.5f);
		float du = nearX - (static_cast<float>(u) + 0.5f);
		float dv = nearY - (static_cast<float>(v) + 0.5f);

		vec3 color00, color10, color11, color01;
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
};

