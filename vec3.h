#pragma once

#include <ostream>

using namespace std;

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
