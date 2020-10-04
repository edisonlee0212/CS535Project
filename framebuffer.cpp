#include "stdafx.h"

#include "framebuffer.h"
#include "math.h"
#include <iostream>
#include "scene.h"

#include <tiffio.h>


using namespace std;

FrameBuffer::FrameBuffer(int u0, int v0,
                         int width, int height, unsigned int _id) : Fl_Gl_Window(u0, v0, width, height, nullptr)
{
	Width = width;
	Height = height;
	Pixels = new unsigned int[Width * Height];
	ZBuffer = new float[Width * Height];
}

void FrameBuffer::ClearZBuffer() const
{
	for (int uv = 0; uv < Width * Height; uv++)
		ZBuffer[uv] = 0.0f;
}

void FrameBuffer::draw()
{
	glDrawPixels(Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, Pixels);
}

int FrameBuffer::handle(int event)
{
	switch (event)
	{
	case FL_KEYBOARD:
		{
			KeyboardHandle();
			return 0;
		}
	default:
		break;
	}
	return 0;
}

void FrameBuffer::KeyboardHandle()
{
	int key = Fl::event_key();
	switch (key)
	{
	case FL_Up:
		{
			cerr << "INFO: pressed up key";
			break;
		}
	default:
		cerr << "INFO: do not understand keypress" << endl;
	}
}


void FrameBuffer::Set(int u, int v, unsigned int color)
{
	if (u < 0 || u > Width - 1 || v < 0 || v > Height - 1)
		return;

	Pixels[(Height - 1 - v) * Width + u] = color;
}

void FrameBuffer::SetZ(int u, int v, float z, unsigned color)
{
	if (u < 0 || v < 0 || u > Width - 1 || v > Height - 1)
		return;

	int uv = (Height - 1 - v) * Width + u;
	if (ZBuffer[uv] > z)
		return;
	ZBuffer[uv] = z;

	Set(u, v, color);
}

void FrameBuffer::SetBGR(unsigned int bgr)
{
	for (int uv = 0; uv < Width * Height; uv++)
		Pixels[uv] = bgr;
}

// load a tiff image to pixel buffer
void FrameBuffer::LoadTiff(char* fileName)
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
	if (Width != width || Height != height)
	{
		Width = width;
		Height = height;
		delete[] Pixels;
		Pixels = new unsigned int[Width * Height];
		size(Width, Height);
		glFlush();
		glFlush();
	}

	if (TIFFReadRGBAImage(in, Width, Height, Pixels, 0) == 0)
	{
		cerr << "failed to load " << fileName << endl;
	}

	TIFFClose(in);
}

// save as tiff image
void FrameBuffer::SaveAsTiff(char* fileName)
{
	TIFF* out = TIFFOpen(fileName, "w");

	if (out == nullptr)
	{
		cerr << fileName << " could not be opened" << endl;
		return;
	}

	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, Width);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, Height);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	for (uint32 row = 0; row < static_cast<unsigned>(Height); row++)
	{
		TIFFWriteScanline(out, &Pixels[(Height - row - 1) * Width], row);
	}

	TIFFClose(out);
}

void FrameBuffer::SetChecker(unsigned int col0, unsigned int col1, int csize)
{
	for (int v = 0; v < Height; v++)
	{
		for (int u = 0; u < Width; u++)
		{
			int cv = v / csize;
			int cu = u / csize;
			if ((cu + cv) % 2)
			{
				Set(u, v, col0);
			}
			else
			{
				Set(u, v, col1);
			}
		}
	}
}

int FrameBuffer::Farther(int u, int v, float currz)
{
	if (u < 0 || u > Width - 1 || v < 0 || v > Height - 1)
		return 1;
	int uv = (Height - 1 - v) * Width + u;
	if (currz < ZBuffer[uv])
		return 1;
	ZBuffer[uv] = currz;
	return 0;
}

void FrameBuffer::Draw2DSegment(vec3 p0, vec3 c0, vec3 p1, vec3 c1)
{
	float du = fabsf((p0 - p1)[0]);
	float dv = fabsf((p0 - p1)[1]);
	int stepsN;
	if (du < dv)
	{
		stepsN = 1 + static_cast<int>(dv);
	}
	else
	{
		stepsN = 1 + static_cast<int>(du);
	}
	for (int i = 0; i <= stepsN; i++)
	{
		vec3 cp, cc;
		cp = p0 + (p1 - p0) * static_cast<float>(i) / static_cast<float>(stepsN);
		// cp[2] depth (one over Width) at current pixel
		int u = static_cast<int>(cp[0]), v = static_cast<int>(cp[1]);
		if (Farther(u, v, cp[2]))
			continue;
		cc = c0 + (c1 - c0) * static_cast<float>(i) / static_cast<float>(stepsN);
		Set(u, v, cc.GetColor());
	}
}

void FrameBuffer::DrawSquarePoint(float uf, float vf, int psize, unsigned int color)
{
	int u = static_cast<int>(uf);
	int v = static_cast<int>(vf);
	for (int cv = v - psize / 2; cv <= v + psize / 2; cv++)
		for (int cu = u - psize / 2; cu <= u + psize / 2; cu++)
			Set(cu, cv, color);
}


void FrameBuffer::Draw3DSegment(vec3 from, vec3 to, Camera* ppc, vec3 c0, vec3 c1)
{
	vec3 p0, p1;
	if (!ppc->Project(from, p0))
		return;
	if (!ppc->Project(to, p1))
		return;

	Draw2DSegment(p0, c0, p1, c1);
}
