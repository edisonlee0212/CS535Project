#pragma once

#include <ostream>

using namespace std;

struct vec2
{
	vec2()
	{
		value[0] = 0;
		value[1] = 0;
	}

	vec2(float x, float y)
	{
		value[0] = x;
		value[1] = y;
	}
	float value[2];

	float& operator[](int i)
	{
		return value[i];
	}

	vec2 operator+(vec2 v1)
	{
		vec2 ret;
		vec2& v0 = *this;
		ret[0] = v0[0] + v1[0];
		ret[1] = v0[1] + v1[1];
		return ret;
	}

	vec2 operator-(vec2 v1)
	{
		vec2 ret;
		vec2& v0 = *this;
		ret[0] = v0[0] - v1[0];
		ret[1] = v0[1] - v1[1];
		return ret;
	}

	friend ostream& operator<<(ostream& os, vec2 v)
	{
		return os << v[0] << " " << v[1] << endl;
	}

	float operator*(vec2 v1)
	{
		vec2& v0 = *this;
		float ret = v0[0] * v1[0] + v0[1] * v1[1];
		return ret;
	}

	vec2 operator*(float scf) const
	{
		vec2 ret(*this);
		ret[0] = ret[0] / scf;
		ret[1] = ret[1] / scf;
		return ret;
	}

	float Length()
	{
		vec2& v0 = *this;
		return sqrtf(v0 * v0);
	}

	vec2 Normalized() const
	{
		vec2 ret(*this);
		return ret / ret.Length();
	}

	vec2 operator/(float scf) const
	{
		vec2 ret(*this);
		ret[0] = ret[0] / scf;
		ret[1] = ret[1] / scf;
		return ret;
	}
};


struct vec3
{
	vec3()
	{
		value[0] = 0;
		value[1] = 0;
		value[2] = 0;
	}

	vec3(float x, float y, float z)
	{
		value[0] = x;
		value[1] = y;
		value[2] = z;
	}

	vec3(vec2 xy)
	{
		value[0] = xy[0];
		value[1] = xy[1];
		value[2] = 0;
	}
	float value[3];
	float& operator[](int i);
	vec3 operator+(vec3 v1);
	vec3 operator-(vec3 v1);
	friend ostream& operator<<(ostream& os, vec3 v);
	float operator*(vec3 v1);
	vec3 operator*(float scf) const;
	float Length();
	vec3 Normalized() const;
	vec3 operator/(float scf) const;
	vec3 operator^(vec3 v2);
	void SetFromColor(unsigned int color);
	unsigned int GetColor();
	vec3 RotatePoint(vec3 aO, vec3 axisDir, float theta);
	vec3 RotateVector(vec3 axisDir, float theta);
};
