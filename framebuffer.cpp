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
	Resize(width, height);
}

void FrameBuffer::Resize(int width, int height)
{
	Width = width;
	Height = height;
	Pixels.resize(Width * Height);
	ZBuffer.resize(Width * Height);
	size_t count = Width * Height;
	std::vector<std::mutex> list(count);
	ZBufferLocks.swap(list);
}

void FrameBuffer::ClearZBuffer()
{
	memset(&ZBuffer[0], 0, ZBuffer.size() * sizeof(float));
}

void FrameBuffer::draw()
{
	if (!EnableGPURendering) {
		glDrawPixels(Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, Pixels.data());
	}else
	{
		glReadPixels(0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, Pixels.data());
	}
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
	cout << "Pressed: " << std::to_string(key) << endl;
}

void FrameBuffer::SetBGR(unsigned int bgr)
{
	memset(&Pixels[0], -1, Pixels.size() * sizeof(unsigned));
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
		Resize(width, height);
		size(Width, Height);
		glFlush();
		glFlush();
	}

	if (TIFFReadRGBAImage(in, Width, Height, Pixels.data(), 0) == 0)
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

bool FrameBuffer::Farther(int u, int v, float z)
{
	if (u < 0 || u > Width - 1 || v < 0 || v > Height - 1)
		return false;
	int uv = (Height - 1 - v) * Width + u;
	std::lock_guard<mutex> lock(ZBufferLocks[uv]);
	if (z < ZBuffer[uv])
		return false;
	ZBuffer[uv] = z;
	return true;
}

void FrameBuffer::ProjectImage(Camera* camera, Camera* projCamera, FrameBuffer* fb, Texture* tex)
{
	int resolutionX = fb->Width;
	int resolutionY = fb->Height;
	for(int i = 0; i < resolutionX; i++)
	{
		for(int j = 0; j < resolutionY; j++)
		{
			if (tex->IsTransparentI(i, j)) continue;
			vec3 proj = vec3(i, j, fb->GetZ(i, j));
			vec3 pos = projCamera->UnProject(proj);
			vec3 camProj;
			camera->Project(pos, camProj);
			Set(camProj[0], camProj[1], tex->Get(i, j));
		}
	}
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
