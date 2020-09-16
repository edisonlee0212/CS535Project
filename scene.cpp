#include "stdafx.h"

#include "scene.h"

#include "vec3.h"
#include "mat3.h"
#include "Camera.h"
#include "Mesh.h"

Scene* scene;

using namespace std;

#include <iostream>

Scene::Scene()
{
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
	ppc = new Camera(hfov, fb->Width, fb->Height);

	TMeshes.push_back(new Mesh());
	TMeshes.push_back(new Mesh());
	
	TMeshes[0]->LoadBin("geometry/teapot1K.bin");
	TMeshes[0]->SetCenter(vec3(0.0f, 0.0f, -300.0f));

	TMeshes[1]->LoadBin("geometry/teapot1K.bin");
	TMeshes[1]->SetCenter(vec3(50.0f, 0.0f, -200.0f));

	Render();
}

void Scene::Render() const
{
	fb->SetBGR(0xFFFFFFFF);
	fb->ClearZB();

	for (const auto& mesh : TMeshes)
	{
		if (!mesh->Enabled)
			continue;
		//TMeshes[tmi].DrawWireFrame(fb, ppc, 0xFF000000);
		mesh->DrawFilled(fb, ppc);
	}

	fb->redraw();
}

void Scene::DBG() const
{
	{
		vec3 target = TMeshes[0]->GetCenter();
		vec3 center = vec3(20.0f, 50.0f, -30.0f);
		ppc->SetPose(center, target, vec3(0.0f, 1.0f, 0.0f));
		Render();
		return;

		vec3 aDir(0.0f, 1.0f, 0.0f);
		for (int i = 0; i < 100; i++)
		{
			Render();
			Fl::check();
			//			TMeshes[1].Rotate(tcenter, aDir, 1.0f);
			ppc->PanLeftRight(1.0f);
		}
		return;
	}

	{
		fb->SaveAsTiff("mydbg/zb.tif");
		return;
	}

	{
		TMeshes[0]->Enabled = false;
		int fN = 300;
		float tstep = .1f;
		for (int fi = 0; fi < fN; fi++)
		{
			Render();
			Fl::check();
			ppc->TranslateRightLeft(-tstep);
		}
		return;
	}


	{
		int w = fb->Width;
		int h = fb->Height;
		float hfov = 90.0f;
		Camera ppc(hfov, w, h);
		vec3 cc(0.0f, 0.0f, -100.0f);
		unsigned int color = 0xFF000000;
		float sideLength = 60.0f;
		Mesh tm;
		tm.SetToCube(cc, sideLength, 0xFF0000FF, 0xFF000000);
		int fN = 300;
		float tstep = .1f;
		for (int fi = 0; fi < fN; fi++)
		{
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
		int w = fb->Width;
		int h = fb->Height;
		float hfov = 90.0f;
		Camera ppc(hfov, w, h);
		vec3 P(0.0f, 0.0f, -100.0f);

		vec3 uP, p;
		ppc.Project(P, p);
		uP = ppc.UnProject(p);
		cerr << uP;

		fb->SetBGR(0xFFF0000);
		vec3 tr(static_cast<float>(w), 0.0f, 1.0f);
		vec3 trP = ppc.UnProject(tr);
		vec3 ptr;
		ppc.Project(trP, ptr);
		fb->DrawSquarePoint(ptr[0], ptr[1], 13, 0xFF00FF00);
		fb->redraw();
		return;

		return;

		vec3 Q(0.0f, -10.0f, -50.0f);
		vec3 q;

		fb->SetBGR(0xFFFFFFFF);
		for (int i = 0; i < 10; i++)
		{
			if (!ppc.Project(P, p))
				continue;
			fb->DrawSquarePoint(p[0], p[1], 5, 0xFF000000);

			if (!ppc.Project(Q, q))
				continue;
			fb->DrawSquarePoint(q[0], q[1], 5, 0xFF0000FF);

			fb->redraw();
			Fl::check();
			P = P + vec3(10.0f, 0.0f, 0.0f);
			Q = Q + vec3(10.0f, 0.0f, 0.0f);
		}


		if (ppc.Project(P, p))
		{
			cerr << p << endl;
		}
		else
		{
			cerr << "INFO: point is behind the head" << endl;
		}

		return;
	}

	{
		mat3 m;
		vec3 r0(1.0f, 1.0f, 1.0f);
		vec3 r1(-2.0f, 2.0f, 2.0f);
		vec3 r2(3.0f, -3.0f, 3.0f);
		m[0] = r0;
		m[1] = r1;
		m[2] = r2;
		vec3 v(1.0f, 2.0f, 3.0f);
		vec3 ret = m * v;
		cerr << ret;
		mat3 m1 = m.Inverted();
		cerr << m * m1.GetColumn(0) << m * m1.GetColumn(1) << m * m1.GetColumn(2);
		return;
	}


	{
		mat3 m;
		vec3 v0(1.0f, 3.0f, -1.0f);
		m[0] = v0;
		cerr << m[0] << endl;
		cerr << m[0][2] << endl;
		m[0][2] = 1000.0f;
		cerr << m[0][2] << endl;
		return;
	}

	{
		vec3 v0(2.0f, 2.0f, 2.0f);
		vec3 v1(4.0f, 3.0f, 5.0f);
		cerr << v0 + v1;
		cerr << "v0*v1 " << v0 * v1 << endl;
		cerr << v0.Length() << endl;
		cerr << (v0.Normalized()).Length() << endl;
		cerr << v0;
		return;
	}

	{
		vec3 v;
		v.value[0] = 1.0f;
		v.value[1] = -1.0f;
		v.value[2] = 0.0f;
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
		float uv0[2] = {10.1f, 20.2f};
		float uv1[2] = {510.1f, 420.2f};
		unsigned int col = 0xFF000000;
		int fN = 300;
		for (int fi = 0; fi < fN; fi++)
		{
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
		float uv0[2] = {20.3f, 300.45f};
		float uv1[2] = {420.73f, 100.45f};
		unsigned int col = 0xFF000000;
		//		fb->Draw2DSegment(uv0, uv1, col);
	}
}


void Scene::NewButton()
{
	cerr << "INFO: pressed New Button" << endl;
}
