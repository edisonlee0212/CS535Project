#include "stdafx.h"
#include "Bounds.h"

vec3 Bounds::GetCenter()
{
	return (MinBound + MaxBound) / 2.0f;
}

bool Bounds::Clip(vec3 minClip, vec3 maxClip, int dimsN)
{
	for (int i = 0; i < dimsN; i++)
	{
		if (MinBound[i] > maxClip[i])
			return false;
		if (MaxBound[i] < minClip[i])
			return false;
	}

	for (int i = 0; i < dimsN; i++)
	{
		if (MinBound[i] < minClip[i])
			MinBound[i] = minClip[i];
		if (MaxBound[i] > maxClip[i])
			MaxBound[i] = maxClip[i];
	}

	return true;
}
