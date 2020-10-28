#pragma once
#include <iostream>
#include <mutex>
#include <ostream>
#include <string>
#include <tiffio.h>
#include <vector>


#include "Camera.h"
#include "mat3.h"
#include "vec3.h"

class PointShadowMap
{
	unsigned _Resolution;
	std::vector<float> _Data[6];
	std::vector<std::mutex> _Locks[6];
public:
	unsigned GetResolution() { return _Resolution; }
	PointShadowMap(unsigned resolution = 128) : _Resolution(resolution)
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
		const float absX = abs(dir[0]);
		const float absY = abs(dir[1]);
		const float absZ = abs(dir[2]);
		vec3 Left = vec3(1.0f, 0.0f, 0.0f);
		vec3 Up = vec3(0.0f, -1.0f, 0.0f);
		const float hfovd = 0.5f * 3.14159236f;
		vec3 c = vec3(-static_cast<float>(_Resolution) / 2.0f, static_cast<float>(_Resolution) / 2.0f,
		              -static_cast<float>(_Resolution) / (2 * tanf(hfovd / 2.0f)));
		int index;
		vec3 target;
		vec3 up;
#pragma region Check faces
		if (absX > absY && absX > absZ)
		{
			if (dir[0] > 0)
			{
				//PX
				target = vec3(1.0, 0.0, 0.0);
				up = vec3(0.0, -1.0, 0.0);
				index = 0;
			}
			else
			{
				//NX
				target = vec3(-1.0, 0.0, 0.0);
				up = vec3(0.0, -1.0, 0.0);
				index = 1;
			}
		}
		else if (absY > absZ)
		{
			if (dir[1] > 0)
			{
				//PY
				target = vec3(0.0, 1.0, 0.0);
				up = vec3(0.0, 0.0, 1.0);
				index = 2;
			}
			else
			{
				//NY
				target = vec3(0.0, -1.0, 0.0);
				up = vec3(0.0, 0.0, -1.0);
				index = 3;
			}
		}
		else
		{
			if (dir[2] > 0)
			{
				//PZ
				target = vec3(0.0, 0.0, 1.0);
				up = vec3(0.0, -1.0, 0.0);
				index = 4;
			}
			else
			{
				//NZ
				target = vec3(0.0, 0.0, -1.0);
				up = vec3(0.0, -1.0, 0.0);
				index = 5;
			}
		}
#pragma endregion
#pragma region SetPose
		vec3 newFront = (target - vec3(0.0)).Normalized();
		vec3 newLeft = (newFront ^ up).Normalized();
		vec3 newUp = (newFront ^ newLeft).Normalized();
		float focalLength = (Left ^ Up).Normalized() * c;
		vec3 newc = newFront * focalLength - newLeft * static_cast<float>(_Resolution) / 2.0f - newUp * static_cast<float>(_Resolution) / 2.0f;

		// commit changes to camera
		Left = newLeft;
		Up = newUp;
		c = newc;
#pragma endregion
		vec3 p;
#pragma region Projection
		mat3 m;
		m.SetColumn(0, Left);
		m.SetColumn(1, Up);
		m.SetColumn(2, c);
		vec3 q = m.Inverted() * (dir - vec3(0.0));
		float w = q[2];
		if (w <= 0.0f)
		{
			p[0] = FLT_MAX;
			p[1] = FLT_MAX;
			p[2] = FLT_MAX;
		}
		else
		{
			p[0] = q[0] / q[2];
			p[1] = q[1] / q[2];
			p[2] = 1.0f / w;
		}
#pragma endregion
		int u = p[0] + 0.5f;
		int v = p[1] + 0.5f;
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

