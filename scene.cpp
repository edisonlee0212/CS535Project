#include "stdafx.h"

#include "scene.h"

#include "V3.h"
#include "M33.h"
#include "ppc.h"
#include "TMesh.h"

Scene *scene;

using namespace std;

#include <iostream>

Scene::Scene() {


	gui = new GUI();
	gui->show();

	int u0 = 20;
	int v0 = 20;
	int h = 480;
	int w = 640;

	fb = new FrameBuffer(u0, v0, w, h, 0);
	fb->label("SW frame buffer");
	fb->show();
	fb->redraw();

	gui->uiw->position(u0, v0 + h + 50);

	float hfov = 55.0f;
	ppc = new PPC(hfov, fb->w, fb->h);
	tmeshesN = 2;
	tmeshes = new TMesh[tmeshesN];

	V3 cc(0.0f, 0.0f, -100.0f);
	float sideLength = 60.0f;
	tmeshes[0].SetToCube(cc, sideLength, 0xFF0000FF, 0xFF000000);
	tmeshes[0].OnFlag = 0;

	tmeshes[1].LoadBin("geometry/teapot1K.bin");
	//tmeshes[1].LoadBin("geometry/teapot57K.bin");
	tmeshes[1].SetCenter(V3(0.0f, 0.0f, -140.0f));

	Render();

}

void Scene::Render() {

	fb->SetBGR(0xFFFFFFFF);
	fb->ClearZB();

	for (int tmi = 0; tmi < tmeshesN; tmi++) {
		if (!tmeshes[tmi].OnFlag)
			continue;
		tmeshes[tmi].DrawWireFrame(fb, ppc, 0xFF000000);
	}

	fb->redraw();


}

void Scene::DBG() {


	{

		V3 tcenter = tmeshes[1].GetCenter();
		V3 newC = V3(20.0f, 50.0f, -30.0f);
		ppc->SetPose(newC, tcenter, V3(0.0f, 1.0f, 0.0f));
		Render();
		return;

		V3 aDir(0.0f, 1.0f, 0.0f);
		for (int i = 0; i < 100; i++) {
			Render();
			Fl::check();
//			tmeshes[1].Rotate(tcenter, aDir, 1.0f);
			ppc->PanLeftRight(1.0f);
		}
		return;

	}

	{
		fb->SaveAsTiff("mydbg/zb.tif");
		return;
	}

	{

		tmeshes[0].OnFlag = 0;
		int fN = 300;
		float tstep = .1f;
		for (int fi = 0; fi < fN; fi++) {
			Render();
			Fl::check();
			ppc->TranslateRightLeft(-tstep);
		}
		return;
	}


	{
		int w = fb->w;
		int h = fb->h;
		float hfov = 90.0f;
		PPC ppc(hfov, w, h);
		V3 cc(0.0f, 0.0f, -100.0f);
		unsigned int color = 0xFF000000;
		float sideLength = 60.0f;
		TMesh tm;
		tm.SetToCube(cc, sideLength, 0xFF0000FF, 0xFF000000);
		int fN = 300;
		float tstep = .1f;
		for (int fi = 0; fi < fN; fi++) {
			fb->SetBGR(0xFFFFFFFF);
//			tm.DrawCubeQuadFaces(fb, &ppc, color);
			tm.DrawWireFrame(fb, &ppc, color);
			fb->redraw();
			Fl::check();
			ppc.TranslateRightLeft(-tstep);
//			ppc.TranslateFrontBack(tstep);
		}
		return;
	}



	{
		int w = fb->w;
		int h = fb->h;
		float hfov = 90.0f;
		PPC ppc(hfov, w, h);
		V3 P(0.0f, 0.0f, -100.0f);

		V3 uP, p;
		ppc.Project(P, p);
		uP = ppc.UnProject(p);
		cerr << uP;

		fb->SetBGR(0xFFF0000);
		V3 tr((float)w, 0.0f, 1.0f);
		V3 trP = ppc.UnProject(tr);
		V3 ptr;
		ppc.Project(trP, ptr);
		fb->DrawSquarePoint(ptr[0], ptr[1], 13, 0xFF00FF00);
		fb->redraw();
		return;

		return;

		V3 Q(0.0f, -10.0f, -50.0f);
		V3 q;

		fb->SetBGR(0xFFFFFFFF);
		for (int i = 0; i < 10; i++) {
			if (!ppc.Project(P, p))
				continue;
			fb->DrawSquarePoint(p[0], p[1], 5, 0xFF000000);

			if (!ppc.Project(Q, q))
				continue;
			fb->DrawSquarePoint(q[0], q[1], 5, 0xFF0000FF);

			fb->redraw();
			Fl::check();
			P = P + V3(10.0f, 0.0f, 0.0f);
			Q = Q + V3(10.0f, 0.0f, 0.0f);
		}


		if (ppc.Project(P, p)) {
			cerr << p << endl;
		}
		else {
			cerr << "INFO: point is behind the head" << endl;
		}

		return;

	}

	{

		M33 m;
		V3 r0(1.0f, 1.0f, 1.0f);
		V3 r1(-2.0f, 2.0f, 2.0f);
		V3 r2(3.0f, -3.0f, 3.0f);
		m[0] = r0;
		m[1] = r1;
		m[2] = r2;
		V3 v(1.0f, 2.0f, 3.0f);
		V3 ret = m*v;
		cerr << ret;
		M33 m1 = m.Inverted();
		cerr << m*m1.GetColumn(0) << m*m1.GetColumn(1) << m*m1.GetColumn(2);
		return;
	}


	{
		M33 m;
		V3 v0(1.0f, 3.0f, -1.0f);
		m[0] = v0;
		cerr << m[0] << endl;
		cerr << m[0][2] << endl;
		m[0][2] = 1000.0f;
		cerr << m[0][2] << endl;
		return;
	}

	{

		V3 v0(2.0f, 2.0f, 2.0f);
		V3 v1(4.0f, 3.0f, 5.0f);
		cerr << v0 + v1;
		cerr << "v0*v1 " << v0*v1 << endl;
		cerr << v0.Length() << endl;
		cerr << (v0.Normalized()).Length() << endl;
		cerr << v0;
		return;

	}

	{
		V3 v;
		v.xyz[0] = 1.0f;
		v.xyz[1] = -1.0f;
		v.xyz[2] = 0.0f;
		cerr << v[0] << endl;
		v[0] = 100.0f;
		cerr << v[0] << endl;
		return;

	}

	fb->LoadTiff("mydbg/im.tif");
	fb->redraw();
	return;
	cerr << "INFO: pressed DBG Button" << endl;

	{
		float uv0[2] = { 10.1f, 20.2f };
		float uv1[2] = { 510.1f, 420.2f };
		unsigned int col = 0xFF000000;
		int fN = 300;
		for (int fi = 0; fi < fN; fi++) {
			fb->SetBGR(0xFFFFFFFF);
//			fb->Draw2DSegment(uv0, uv1, cv, cv);
			uv0[1] += 1.0f;
			uv1[1] -= 1.0f;
			fb->redraw();
			Fl::check();
		}
		fb->SaveAsTiff("mydbg/im.tif");
	}

	return;

	{
		fb->SetBGR(0xFF0000FF);
		fb->SetChecker(0xFF000000, 0xFFFFFFFF, 40);
		fb->SetBGR(0xFFFFFFFF);
		float uv0[2] = { 20.3f, 300.45f };
		float uv1[2] = { 420.73f, 100.45f };
		unsigned int col = 0xFF000000;
//		fb->Draw2DSegment(uv0, uv1, col);
	}

}


void Scene::NewButton() {
	cerr << "INFO: pressed New Button" << endl;
}
