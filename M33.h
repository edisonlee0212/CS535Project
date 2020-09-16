#pragma once

#include "V3.h"

using namespace std;


class M33 {
public:
	V3 rows[3];
	M33() {};
	V3& operator[](int i);
	V3 operator*(V3 v);
	M33 Inverted();
	V3 GetColumn(int i);
	void SetColumn(int i, V3 v);
	void SetRotationAboutY(float theta);
};

// m00 m01 m02    v0    r0*v
// m10 m11 m12  x v1 =  r1*v
// m20 m21 m22    v2    r2*v


//  r0					r0*v
//  r1   x  v        =	r1*v
//  r2					r2*v