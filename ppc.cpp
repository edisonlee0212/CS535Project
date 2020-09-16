#include "stdafx.h"

#include "ppc.h"

#include "M33.h"

#include <iostream>

PPC::PPC(float hfov, int _w, int _h) {

	w = _w;
	h = _h;
	C = V3(0.0f, 0.0f, 0.0f);
	a = V3(1.0f, 0.0f, 0.0f);
	b = V3(0.0f, -1.0f, 0.0f);
	float hfovd = hfov / 180.0f * 3.14159236f;
	c = V3(-(float)w / 2.0f, (float)h / 2, -(float)w / (2 * tanf(hfovd / 2.0f)));

}

int PPC::Project(V3 P, V3 &p) {

	M33 M;
	M.SetColumn(0, a);
	M.SetColumn(1, b);
	M.SetColumn(2, c);
	V3 q = M.Inverted()*(P - C);
	float w = q[2];
	if (w <= 0.0f)
		return 0;

	p[0] = q[0] / q[2];
	p[1] = q[1] / q[2];
	p[2] = 1.0f / w;
	return 1;
}

V3 PPC::UnProject(V3 p) {

	V3 ret;
	ret = C + (a*p[0] + b*p[1] + c)/p[2];
	return ret;

}


void PPC::TranslateRightLeft(float tstep) {

	V3 rightDir = a.Normalized();
	C = C + rightDir*tstep;

}

void PPC::TranslateFrontBack(float tstep) {

	V3 tDir = (a^b).Normalized();
	C = C + tDir*tstep;

}


void PPC::PanLeftRight(float rstep) {

	V3 adir = b.Normalized()*-1.0f;
	a = a.RotateVector(adir, rstep);
	c = c.RotateVector(adir, rstep);

}

void PPC::SetPose(V3 newC, V3 lookAtPoint, V3 upGuidance) {

	V3 newvd = (lookAtPoint - newC).Normalized();
	V3 newa = (newvd ^ upGuidance).Normalized();
	V3 newb = (newvd^newa).Normalized();
	float focalLength = (a^b).Normalized()*c;
	V3 newc = newvd*focalLength - newa*(float)w / 2.0f - newb*(float)h / 2.0f;


	// commit changes to camera
	a = newa;
	b = newb;
	c = newc;
	C = newC;

}

void PPC::Interpolate(PPC *ppc0, PPC *ppc1, int i, int n) {

	cerr << "INFO: not yet implemented" << endl;
	PPC &ppc = *this;
	float scf = (float)i / (float)(n - 1);
	ppc.C = ppc0->C + (ppc1->C - ppc0->C)*scf;
	// build lookatpoint and upguidance by interpolation, call setpose to create interpolated camera

}