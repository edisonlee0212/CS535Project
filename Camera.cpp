#include "stdafx.h"

#include "Camera.h"

#include "mat3.h"

#include <iostream>

Camera::Camera(float hfov, int width, int height)
{
	Width = width;
	Height = height;
	
	Left = vec3(1.0f, 0.0f, 0.0f);
	Up = vec3(0.0f, -1.0f, 0.0f);
	float hfovd = hfov / 180.0f * 3.14159236f;
	c = vec3(-static_cast<float>(Width) / 2.0f, static_cast<float>(Height) / 2.0f, -static_cast<float>(Width) / (2 * tanf(hfovd / 2.0f)));

	Center = vec3(0.0f, 0.0f, 0.0f);
}

vec3 Camera::GetFront()
{
	return (Left ^ Up).Normalized();
}

bool Camera::Project(vec3 P, vec3& p) const
{
	mat3 m;
	m.SetColumn(0, Left);
	m.SetColumn(1, Up);
	m.SetColumn(2, c);
	vec3 q = m.Inverted() * (P - Center);
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
	ret = Center + (Left * p[0] + Up * p[1] + c) / p[2];
	return ret;
}


void Camera::TranslateRightLeft(float tStep)
{
	vec3 rightDir = Left.Normalized();
	Center = Center + rightDir * tStep;
}

void Camera::TranslateFrontBack(float tStep)
{
	vec3 tDir = (Left ^ Up).Normalized();
	Center = Center + tDir * tStep;
}


void Camera::PanLeftRight(float rStep)
{
	vec3 adir = Up.Normalized() * -1.0f;
	Left = Left.RotateVector(adir, rStep);
	c = c.RotateVector(adir, rStep);
}

void Camera::SetPose(vec3 center, vec3 target, vec3 up)
{
	vec3 newFront = (target - center).Normalized();
	vec3 newLeft = (newFront ^ up).Normalized();
	vec3 newUp = (newFront ^ newLeft).Normalized();
	float focalLength = (Left ^ Up).Normalized() * c;
	vec3 newc = newFront * focalLength - newLeft * static_cast<float>(Width) / 2.0f - newUp * static_cast<float>(Height) / 2.0f;

	// commit changes to camera
	Left = newLeft;
	Up = newUp;
	c = newc;
	Center = center;
}

void Camera::Interpolate(Camera* ppc0, Camera* ppc1, int i, int n)
{
	Camera& ppc = *this;
	float scf = static_cast<float>(i) / static_cast<float>(n - 1);
	vec3 newCenter = ppc0->Center + (ppc1->Center - ppc0->Center) * scf;
	// build lookatpoint and upguidance by interpolation, call setpose to create interpolated camera
	vec3 newFront = ppc0->GetFront() + (ppc1->GetFront() - ppc0->GetFront()) * scf;
	vec3 newUp = (ppc0->Up + (ppc1->Up - ppc0->Up) * scf) * -1.0f;
	SetPose(newCenter, newCenter + newFront, newUp);
}

void Camera::SetFov(float value)
{
	float hfovd = value / 180.0f * 3.14159236f;
	c = vec3(-static_cast<float>(Width) / 2.0f, static_cast<float>(Height) / 2.0f, -static_cast<float>(Width) / (2 * tanf(hfovd / 2.0f)));
}

void Camera::Pan(float theta)
{
	Left = Left.RotateVector(Up.Normalized() * -1.0f, theta);
	c = c.RotateVector(Up.Normalized() * -1.0f, theta);
}

void Camera::Tilt(float theta)
{
	Up = Up.RotateVector(Left.Normalized() * -1.0f, theta);
	c = c.RotateVector(Left.Normalized() * -1.0f, theta);
}

void Camera::Roll(float theta)
{
	Left = Left.RotateVector(GetFront(), theta);
	Up = Up.RotateVector(GetFront(), theta);
	c = c.RotateVector(GetFront(), theta);
}
