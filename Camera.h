#pragma once

#include "vec3.h"

class Camera
{
public:
	vec3 Left, Up, c, Center;
	int Width, Height;
	Camera(float hfov, int width, int height);
	vec3 GetFront();
	bool Project(vec3 P, vec3& p) const;
	vec3 UnProject(vec3 p);
	void TranslateRightLeft(float tStep);
	void TranslateFrontBack(float tStep);
	void PanLeftRight(float rStep);
	void SetPose(vec3 center, vec3 target, vec3 up);
	void Interpolate(Camera* ppc0, Camera* ppc1, int i, int n);
	void SetFov(float value);
	void Pan(float theta);
	void Tilt(float theta);
	void Roll(float theta);
	float GetF()
	{
		return c * (Left ^ Up).Normalized();
	}
	void SetIntrinsicsHW();
	void SetExtrinsicsHW();
};
