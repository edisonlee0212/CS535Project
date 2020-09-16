#pragma once
#include "vec3.h"

struct Bounds
{
	vec3 MinBound;
	vec3 MaxBound;
	vec3 GetCenter();
	bool Clip(vec3 minClip, vec3 maxClip, int dimsN);
};
