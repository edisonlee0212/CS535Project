#include "stdafx.h"
#include "Texture.h"
#include "stb_image.h"
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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _Width, _Height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, _Pixels.data());
	Loaded = true;
}
