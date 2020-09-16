#include "stdafx.h"
#include "aabb.h"

V3 AABB::GetCenter()
{
	return (MinBound + MaxBound) / 2.0f;
}
