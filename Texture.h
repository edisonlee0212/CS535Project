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
	vec3 Bilinear(float x, float y)
	{
		return vec3(0,0,0);
	}
};

