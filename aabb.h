#pragma once
#include "V3.h"
struct AABB
{
	V3 MinBound;
	V3 MaxBound;
	V3 GetCenter();
};

