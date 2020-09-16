#include "stdafx.h"

#include "M33.h"

V3 M33::operator*(V3 v) {

	V3 ret;
	M33 &m = *this;
	ret[0] = m[0] * v;
	ret[1] = m[1] * v;
	ret[2] = m[2] * v;
	return ret;
}


V3& M33::operator[](int i) {

	return rows[i];

}

M33 M33::Inverted() {

	M33 ret;
	V3 a = GetColumn(0), b = GetColumn(1), c = GetColumn(2);
	V3 _a = b ^ c; _a = _a / (a * _a);
	V3 _b = c ^ a; _b = _b / (b * _b);
	V3 _c = a ^ b; _c = _c / (c * _c);
	ret[0] = _a;
	ret[1] = _b;
	ret[2] = _c;

	return ret;

}

V3 M33::GetColumn(int i) {

	M33 &m = *this;
	return V3(m[0][i], m[1][i], m[2][i]);

}

void M33::SetColumn(int i, V3 v) {

	M33&m = *this;
	m[0][i] = v[0];
	m[1][i] = v[1];
	m[2][i] = v[2];

}

void M33::SetRotationAboutY(float thetad) {

	float theta = thetad * 3.1415629f / 180.0f;
	M33 &m = *this;
	m[0] = V3(cosf(theta), 0.0f, sin(theta));
	m[1] = V3(0.0f, 1.0f, 0.0f);
	m[2] = V3(-sinf(theta), 0.0f, cos(theta));

}
