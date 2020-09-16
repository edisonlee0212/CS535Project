#include "stdafx.h"
#include "Bounds.h"

vec3 Bounds::GetCenter()
{
	return (MinBound + MaxBound) / 2.0f;
}

bool Bounds::Clip(vec3 minc, vec3 maxc, int dimsN)
{
	for (int i = 0; i < dimsN; i++)
	{
		if (MinBound[i] > maxc[i])
			return false;
		if (MaxBound[i] < minc[i])
			return false;
	}

	for (int i = 0; i < dimsN; i++)
	{
		if (MinBound[i] < minc[i])
			MinBound[i] = minc[i];
		if (MaxBound[i] > maxc[i])
			MaxBound[i] = maxc[i];
	}

	return true;
}
