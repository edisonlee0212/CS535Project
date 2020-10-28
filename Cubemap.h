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
class Cubemap
{
	unsigned _Resolution;
	std::vector<unsigned> _Data[6];
	unsigned Get(int u, int v, int index)
	{
		return _Data[index][(_Resolution - v - 1) * _Resolution + u];
	}
	unsigned Bilinear(float x, float y, int index)
	{
		float nearX = _Resolution * x;
		float nearY = _Resolution * y;

		if (nearY < 0.5f) nearY = 0.5f;
		int u = static_cast<int>(nearX - 0.5f);
		int v = static_cast<int>(nearY - 0.5f);
		float du = nearX - (static_cast<float>(u) + 0.5f);
		float dv = nearY - (static_cast<float>(v) + 0.5f);
		vec3 color00, color10, color11, color01;
		if (u < 0) u = 0;
		if (v < 0) v = 0;
		if (u > _Resolution - 2) u = _Resolution - 2;
		if (v > _Resolution - 2) v = _Resolution - 2;

		color00.SetFromColor(Get(u, v, index));
		color10.SetFromColor(Get(u + 1, v, index));
		color11.SetFromColor(Get(u + 1, v + 1, index));
		color01.SetFromColor(Get(u, v + 1, index));

		vec3 ret;
		ret = color00 * (1.0f - du) * (1.0f - dv)
			+ color10 * du * (1.0f - dv)
			+ color11 * du * dv
			+ color01 * (1.0f - du) * dv;

		return ret.GetColor();
	}
public:
	unsigned Get(vec3 dir)
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
	unsigned GetBilinear(vec3 dir)
	{
#pragma region LookUp
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
#pragma endregion
		return Bilinear(p[0] / _Resolution, p[1] / _Resolution, index);
	}
	Cubemap(unsigned resolution = 512) : _Resolution(resolution)
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
		for (int i = 0; i < 6; i++)
		{
			_Data[i].resize(resolution * resolution);
			size_t count = resolution * resolution;
		}

	}
	void LoadTiffs(std::string px, std::string nx, std::string py, std::string ny, std::string pz, std::string nz);
};

