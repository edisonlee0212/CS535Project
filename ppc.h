#pragma once

#include "v3.h"

class PPC {
public:
	V3 a, b, c, C;
	int w, h;
	PPC(float hfov, int _w, int _h);
	int Project(V3 P, V3 &p);
	V3 UnProject(V3 p);
	void TranslateRightLeft(float tstep);
	void TranslateFrontBack(float tstep);
	void PanLeftRight(float rstep);
	void SetPose(V3 newC, V3 lookAtPoint, V3 upGuidance);
	void Interpolate(PPC *ppc0, PPC *ppc1, int i, int n);
};