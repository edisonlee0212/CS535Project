#include "stdafx.h"

#include "mat3.h"

vec3 mat3::operator*(vec3 v)
{
	vec3 ret;
	mat3& m = *this;
	ret[0] = m[0] * v;
	ret[1] = m[1] * v;
	ret[2] = m[2] * v;
	return ret;
}


vec3& mat3::operator[](int i)
{
	return value[i];
}

mat3 mat3::Inverted()
{
	mat3 ret;
	vec3 a = GetColumn(0), b = GetColumn(1), c = GetColumn(2);
	vec3 _a = b ^ c;
	_a = _a / (a * _a);
	vec3 _b = c ^ a;
	_b = _b / (b * _b);
	vec3 _c = a ^ b;
	_c = _c / (c * _c);
	ret[0] = _a;
	ret[1] = _b;
	ret[2] = _c;

	return ret;
}

vec3 mat3::GetColumn(int i)
{
	mat3& m = *this;
	return vec3(m[0][i], m[1][i], m[2][i]);
}

void mat3::SetColumn(int i, vec3 v)
{
	mat3& m = *this;
	m[0][i] = v[0];
	m[1][i] = v[1];
	m[2][i] = v[2];
}

void mat3::SetRotationAboutY(float thetad)
{
	float theta = thetad * 3.1415629f / 180.0f;
	mat3& m = *this;
	m[0] = vec3(cosf(theta), 0.0f, sin(theta));
	m[1] = vec3(0.0f, 1.0f, 0.0f);
	m[2] = vec3(-sinf(theta), 0.0f, cos(theta));
}
