#include "stdafx.h"

#include "vec3.h"
#include "mat3.h"

float& vec3::operator[](int i)
{
	return value[i];
}

vec3 vec3::operator+(vec3 v1)
{
	vec3 ret;
	vec3& v0 = *this;
	ret[0] = v0[0] + v1[0];
	ret[1] = v0[1] + v1[1];
	ret[2] = v0[2] + v1[2];
	return ret;
}

vec3 vec3::operator-(vec3 v1)
{
	vec3 ret;
	vec3& v0 = *this;
	ret[0] = v0[0] - v1[0];
	ret[1] = v0[1] - v1[1];
	ret[2] = v0[2] - v1[2];
	return ret;
}


ostream& operator<<(ostream& os, vec3 v)
{
	return os << v[0] << " " << v[1] << " " << v[2] << endl;
}

float vec3::operator*(vec3 v1)
{
	vec3& v0 = *this;
	float ret = v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
	return ret;
}

float vec3::Length()
{
	vec3& v0 = *this;
	return sqrtf(v0 * v0);
}

vec3 vec3::operator/(float scf) const
{
	vec3 ret(*this);
	ret[0] = ret[0] / scf;
	ret[1] = ret[1] / scf;
	ret[2] = ret[2] / scf;
	return ret;
}

vec3 vec3::Normalized() const
{
	vec3 ret(*this);
	return ret / ret.Length();
}

vec3 vec3::operator^(vec3 v2)
{
	vec3& v1 = *this;
	vec3 ret(v1[1] * v2[2] - v1[2] * v2[1],
	       v1[2] * v2[0] - v1[0] * v2[2],
	       v1[0] * v2[1] - v1[1] * v2[0]);
	return ret;
}


vec3 vec3::operator*(float scf)
{
	vec3 ret(*this);
	ret[0] *= scf;
	ret[1] *= scf;
	ret[2] *= scf;
	return ret;
}

void vec3::SetFromColor(unsigned int color)
{
	auto* rgb = (unsigned char*)&color;
	vec3& v = *this;
	v[0] = static_cast<float>(rgb[0]) / 255.0f;
	v[1] = static_cast<float>(rgb[1]) / 255.0f;
	v[2] = static_cast<float>(rgb[2]) / 255.0f;
}

unsigned int vec3::GetColor()
{
	unsigned int ret;
	vec3& v = *this;
	unsigned char rgb[3];
	rgb[0] = static_cast<unsigned char>(255.0f * v[0]);
	rgb[1] = static_cast<unsigned char>(255.0f * v[1]);
	rgb[2] = static_cast<unsigned char>(255.0f * v[2]);
	ret = 0xFF000000 + rgb[2] * 256 * 256 + rgb[1] * 256 + rgb[0];
	return ret;
}

vec3 vec3::RotateVector(vec3 axisDir, float theta)
{
	return RotatePoint(vec3(0.0f, 0.0f, 0.0f), axisDir, theta);
}

vec3 vec3::RotatePoint(vec3 aO, vec3 axisDir, float theta)
{
	vec3 aux;
	if (fabsf(axisDir[0]) < fabsf(axisDir[1]))
		aux = vec3(1.0f, 0.0f, 0.0f);
	else
		aux = vec3(0.0f, 1.0f, 0.0f);

	mat3 lcs;
	lcs[0] = (aux ^ axisDir).Normalized();
	lcs[1] = axisDir;
	lcs[2] = (lcs[0] ^ lcs[1]).Normalized();

	vec3& P = *this;
	vec3 lP = lcs * (P - aO);

	mat3 roty;
	roty.SetRotationAboutY(theta);
	vec3 rlP = roty * lP;

	vec3 rP = lcs.Inverted() * rlP + aO;
	return rP;
}
