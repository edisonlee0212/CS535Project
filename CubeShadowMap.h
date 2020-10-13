#pragma once
#include <iostream>
#include <mutex>
#include <ostream>
#include <string>
#include <tiffio.h>
#include <vector>


#include "Camera.h"
#include "vec3.h"

class CubeShadowMap
{
	unsigned _Resolution;
	std::vector<float> _Data[6];
	std::vector<std::mutex> _Locks[6];
public:
	unsigned GetResolution() { return _Resolution; }
	CubeShadowMap(unsigned resolution = 128) : _Resolution(resolution)
	{
		Reallocate(resolution);
	}
	void Clear()
	{
		memset(&_Data[0][0], 0, _Data[0].size() * sizeof(float));
		memset(&_Data[1][0], 0, _Data[1].size() * sizeof(float));
		memset(&_Data[2][0], 0, _Data[2].size() * sizeof(float));
		memset(&_Data[3][0], 0, _Data[3].size() * sizeof(float));
		memset(&_Data[4][0], 0, _Data[4].size() * sizeof(float));
		memset(&_Data[5][0], 0, _Data[5].size() * sizeof(float));
	}
	void Reallocate(unsigned resolution)
	{
		_Resolution = resolution;
		for(int i = 0; i < 6; i++)
		{
			_Data[i].resize(resolution * resolution);
			size_t count = resolution * resolution;
			std::vector<std::mutex> list(count);
			_Locks[i].swap(list);
		}
		
	}
	void SetZ(int u, int v, float z, int index)
	{
		if (u < 0 || v < 0 || u > _Resolution - 1 || v > _Resolution - 1)
			return;

		int uv = (_Resolution - 1 - v) * _Resolution + u;
		//std::lock_guard<mutex> lock(_Locks[index][uv]);
		if (_Data[index][uv] > z)
			return;
		_Data[index][uv] = z;
	}
	float GetZ(vec3 dir)
	{
		float absX = abs(dir[0]);
		float absY = abs(dir[1]);
		float absZ = abs(dir[2]);
		Camera camera(90, _Resolution, _Resolution);
		int index = 0;
#pragma region Check faces
		if (absX > absY && absX > absZ)
		{
			if (dir[0] > 0)
			{
				//PX
				camera.SetPose(vec3(0.0), vec3(1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0));
				index = 0;
			}
			else
			{
				//NX
				camera.SetPose(vec3(0.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0));
				index = 1;
			}
		}
		else if (absY > absZ)
		{
			if (dir[1] > 0)
			{
				//PY
				camera.SetPose(vec3(0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));
				index = 2;
			}
			else
			{
				//NY
				camera.SetPose(vec3(0.0), vec3(0.0, -1.0, 0.0), vec3(0.0, 0.0, -1.0));
				index = 3;
			}
		}
		else
		{
			if (dir[2] > 0)
			{
				//PZ
				camera.SetPose(vec3(0.0), vec3(0.0, 0.0, 1.0), vec3(0.0, -1.0, 0.0));
				index = 4;
			}
			else
			{
				//NZ
				camera.SetPose(vec3(0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, -1.0, 0.0));
				index = 5;
			}
		}
#pragma endregion
		
		vec3 proj;
		camera.Project(dir, proj);
		int u = proj[0] + 0.5f;
		int v = proj[1] + 0.5f;
		if (u < 0) u = 0;
		if (v < 0) v = 0;
		if (u > _Resolution - 1) u = _Resolution - 1;
		if (v > _Resolution - 1) v = _Resolution - 1;
		int uv = (_Resolution - 1 - v) * _Resolution + u;
		
		return _Data[index][uv];
	}
	void SaveAsTiff(std::string fileName) const
	{
		for (int index = 0; index < 6; index++) {
			std::vector<unsigned> output;
			output.resize(_Resolution * _Resolution);
			for(int i = 0; i < _Resolution * _Resolution; i++)
			{
				output[i] = vec3(_Data[index][i] * 20).GetColor();
			}
			TIFF* out = TIFFOpen((fileName + std::to_string(index) + ".tif").c_str(), "w");

			if (out == nullptr)
			{
				std::cerr << fileName << " could not be opened" << std::endl;
				return;
			}

			TIFFSetField(out, TIFFTAG_IMAGEWIDTH, _Resolution);
			TIFFSetField(out, TIFFTAG_IMAGELENGTH, _Resolution);
			TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
			TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
			TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
			TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
			TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

			for (uint32 row = 0; row < static_cast<unsigned>(_Resolution); row++)
			{
				TIFFWriteScanline(out, (void*)&output[(_Resolution - row - 1) * _Resolution], row);
			}

			TIFFClose(out);
		}
	}
};

