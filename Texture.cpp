#include "stdafx.h"
#include "Texture.h"

void Texture::LoadTiff(std::string fileName)
{
	TIFF* in = TIFFOpen(fileName.c_str(), "r");
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
	SetAllTransparency(false);
	glGenTextures(1, &_ID);
	glBindTexture(GL_TEXTURE_2D, _ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _Width, _Height, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, _Pixels.data());
	Loaded = true;
}