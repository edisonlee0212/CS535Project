#include "stdafx.h"

#include "Camera.h"

#include "mat3.h"

#include <iostream>

Camera::Camera(float hfov, int width, int height)
{
	Width = width;
	Height = height;
	
	a = vec3(1.0f, 0.0f, 0.0f);
	b = vec3(0.0f, -1.0f, 0.0f);
	float hfovd = hfov / 180.0f * 3.14159236f;
	c = vec3(-static_cast<float>(Width) / 2.0f, static_cast<float>(Height) / 2.0f, -static_cast<float>(Width) / (2 * tanf(hfovd / 2.0f)));

	C = vec3(0.0f, 0.0f, 0.0f);
}

bool Camera::Project(vec3 P, vec3& p) const
{
	mat3 m;
	m.SetColumn(0, a);
	m.SetColumn(1, b);
	m.SetColumn(2, c);
	vec3 q = m.Inverted() * (P - C);
	float w = q[2];
	if (w <= 0.0f)
	{
		p[0] = FLT_MAX;
		p[1] = FLT_MAX;
		p[2] = FLT_MAX;
		return false;
	}else
	{
		p[0] = q[0] / q[2];
		p[1] = q[1] / q[2];
		p[2] = 1.0f / w;
		return true;
	}
}

vec3 Camera::UnProject(vec3 p)
{
	vec3 ret;
	ret = C + (a * p[0] + b * p[1] + c) / p[2];
	return ret;
}


void Camera::TranslateRightLeft(float tStep)
{
	vec3 rightDir = a.Normalized();
	C = C + rightDir * tStep;
}

void Camera::TranslateFrontBack(float tStep)
{
	vec3 tDir = (a ^ b).Normalized();
	C = C + tDir * tStep;
}


void Camera::PanLeftRight(float rStep)
{
	vec3 adir = b.Normalized() * -1.0f;
	a = a.RotateVector(adir, rStep);
	c = c.RotateVector(adir, rStep);
}

void Camera::SetPose(vec3 center, vec3 target, vec3 up)
{
	vec3 newvd = (target - center).Normalized();
	vec3 newa = (newvd ^ up).Normalized();
	vec3 newb = (newvd ^ newa).Normalized();
	float focalLength = (a ^ b).Normalized() * c;
	vec3 newc = newvd * focalLength - newa * static_cast<float>(Width) / 2.0f - newb * static_cast<float>(Height) / 2.0f;


	// commit changes to camera
	a = newa;
	b = newb;
	c = newc;
	C = center;
}

void Camera::Interpolate(Camera* ppc0, Camera* ppc1, int i, int n)
{
	cerr << "INFO: not yet implemented" << endl;
	Camera& ppc = *this;
	float scf = static_cast<float>(i) / static_cast<float>(n - 1);
	ppc.C = ppc0->C + (ppc1->C - ppc0->C) * scf;
	// build lookatpoint and upguidance by interpolation, call setpose to create interpolated camera
}
