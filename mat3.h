#pragma once

#include "vec3.h"

using namespace std;


struct mat3
{
	vec3 value[3];
	mat3()
	{
	};
	vec3& operator[](int i);
	vec3 operator*(vec3 v);
	mat3 mat3::operator*(mat3 m1) {
		mat3 ret, & m = *this;
		ret.SetColumn(0, m * m1.GetColumn(0));
		ret.SetColumn(1, m * m1.GetColumn(1));
		ret.SetColumn(2, m * m1.GetColumn(2));
		return ret;
	}
	mat3 Inverted();
	vec3 GetColumn(int i);
	void SetColumn(int i, vec3 v);
	void SetRotationAboutY(float theta);
	mat3 Transpose()
	{
		mat3 ret;
		ret[0] = GetColumn(0);
		ret[1] = GetColumn(1);
		ret[2] = GetColumn(2);
		return ret;
	}
};

// m00 m01 m02    v0    r0*v
// m10 m11 m12  x v1 =  r1*v
// m20 m21 m22    v2    r2*v


//  r0					r0*v
//  r1   x  v        =	r1*v
//  r2					r2*v
