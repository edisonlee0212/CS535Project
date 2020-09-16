#include "stdafx.h"

#include <fstream>
#include <iostream>

#include "TMesh.h"

using namespace std;


void TMesh::Allocate(int vertsN, int trisN) {

	VertsN = vertsN;
	TrisN = trisN;
	Verts = new V3[VertsN];
	Colors = new V3[VertsN];
	Normals = new V3[VertsN];
	Tris = new unsigned int[TrisN * 3];
}

void TMesh::RecalculateBoundingBox()
{
	BoundingBox.MaxBound = V3(-999999, -999999, -999999);
	BoundingBox.MinBound = V3(999999, 999999, 999999);
	for (int i = 0; i < VertsN; i++)
	{
		if (BoundingBox.MaxBound[0] < Verts[i][0]) BoundingBox.MaxBound[0] = Verts[i][0];
		if (BoundingBox.MaxBound[1] < Verts[i][1]) BoundingBox.MaxBound[1] = Verts[i][1];
		if (BoundingBox.MaxBound[2] < Verts[i][2]) BoundingBox.MaxBound[2] = Verts[i][2];
		if (BoundingBox.MinBound[0] > Verts[i][0]) BoundingBox.MinBound[0] = Verts[i][0];
		if (BoundingBox.MinBound[1] > Verts[i][1]) BoundingBox.MinBound[1] = Verts[i][1];
		if (BoundingBox.MinBound[2] > Verts[i][2]) BoundingBox.MinBound[2] = Verts[i][2];
	}
}

void TMesh::SetToCube(V3 cc, float sideLength, unsigned int color0, unsigned int color1) {

	VertsN = 8;
	TrisN = 6 * 2;
	Allocate(VertsN, TrisN);

	for (int vi = 0; vi < 4; vi++) {
		Colors[vi].SetFromColor(color0);
		Colors[vi+4].SetFromColor(color1);
	}

	int vi = 0;
	Verts[vi] = cc + V3(-sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	Verts[vi] = cc + V3(-sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	Verts[vi] = cc + V3(+sideLength / 2.0f, -sideLength / 2.0f, +sideLength / 2.0f);
	vi++;
	Verts[vi] = cc + V3(+sideLength / 2.0f, +sideLength / 2.0f, +sideLength / 2.0f);
	vi++;

	Verts[vi] = Verts[vi - 4] + V3(0.0f, 0.0f, -sideLength);
	vi++;
	Verts[vi] = Verts[vi - 4] + V3(0.0f, 0.0f, -sideLength);
	vi++;
	Verts[vi] = Verts[vi - 4] + V3(0.0f, 0.0f, -sideLength);
	vi++;
	Verts[vi] = Verts[vi - 4] + V3(0.0f, 0.0f, -sideLength);
	vi++;

	int tri = 0;
	Tris[3 * tri + 0] = 0;
	Tris[3 * tri + 1] = 1;
	Tris[3 * tri + 2] = 2;
	tri++;
	Tris[3 * tri + 0] = 2;
	Tris[3 * tri + 1] = 3;
	Tris[3 * tri + 2] = 0;
	tri++;
	Tris[3 * tri + 0] = 3;
	Tris[3 * tri + 1] = 2;
	Tris[3 * tri + 2] = 6;
	tri++;
	Tris[3 * tri + 0] = 6;
	Tris[3 * tri + 1] = 7;
	Tris[3 * tri + 2] = 3;
	tri++;
	Tris[3 * tri + 0] = 7;
	Tris[3 * tri + 1] = 6;
	Tris[3 * tri + 2] = 5;
	tri++;
	Tris[3 * tri + 0] = 5;
	Tris[3 * tri + 1] = 4;
	Tris[3 * tri + 2] = 7;
	tri++;
	Tris[3 * tri + 0] = 4;
	Tris[3 * tri + 1] = 5;
	Tris[3 * tri + 2] = 1;
	tri++;
	Tris[3 * tri + 0] = 1;
	Tris[3 * tri + 1] = 0;
	Tris[3 * tri + 2] = 4;
	tri++;
	Tris[3 * tri + 0] = 4;
	Tris[3 * tri + 1] = 0;
	Tris[3 * tri + 2] = 3;
	tri++;
	Tris[3 * tri + 0] = 3;
	Tris[3 * tri + 1] = 7;
	Tris[3 * tri + 2] = 4;
	tri++;
	Tris[3 * tri + 0] = 1;
	Tris[3 * tri + 1] = 5;
	Tris[3 * tri + 2] = 6;
	tri++;
	Tris[3 * tri + 0] = 6;
	Tris[3 * tri + 1] = 2;
	Tris[3 * tri + 2] = 1;
	tri++;
	RecalculateBoundingBox();
}

void TMesh::DrawCubeQuadFaces(FrameBuffer *fb, PPC *ppc, unsigned int color) {

	V3 c0;
	c0.SetFromColor(color);
	for (int si = 0; si < 4; si++) {
		fb->Draw3DSegment(Verts[si], Verts[(si + 1) % 4], ppc, c0, c0);
		fb->Draw3DSegment(Verts[4+si], Verts[4 + (si + 1) % 4], ppc, c0, c0);
		fb->Draw3DSegment(Verts[si], Verts[si + 4], ppc, c0, c0);
	}

}

void TMesh::DrawWireFrame(FrameBuffer *fb, PPC *ppc, unsigned int color) {

	for (int tri = 0; tri < TrisN; tri++) {
		V3 V0 = Verts[Tris[3 * tri + 0]];
		V3 V1 = Verts[Tris[3 * tri + 1]];
		V3 V2 = Verts[Tris[3 * tri + 2]];
		V3 c0 = Colors[Tris[3 * tri + 0]];
		V3 c1 = Colors[Tris[3 * tri + 1]];
		V3 c2 = Colors[Tris[3 * tri + 2]];
		fb->Draw3DSegment(V0, V1, ppc, c0, c1);
		fb->Draw3DSegment(V1, V2, ppc, c1, c2);
		fb->Draw3DSegment(V2, V0, ppc, c2, c0);
	}

}

void TMesh::LoadBin(char *filename) {

	ifstream ifs(filename, ios::binary);
	if (ifs.fail()) {
		cerr << "INFO: cannot open file: " << filename << endl;
		return;
	}

	ifs.read((char*)&VertsN, sizeof(int));
	char yn;
	ifs.read(&yn, 1); // always xyz
	if (yn != 'y') {
		cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
		return;
	}
	if (Verts)
		delete Verts;
	Verts = new V3[VertsN];

	ifs.read(&yn, 1); // cols 3 floats
	if (Colors)
		delete Colors;
	Colors = 0;
	if (yn == 'y') {
		Colors = new V3[VertsN];
	}

	ifs.read(&yn, 1); // normals 3 floats
	if (Normals)
		delete Normals;
	Normals = 0;
	if (yn == 'y') {
		Normals = new V3[VertsN];
	}

	ifs.read(&yn, 1); // texture coordinates 2 floats
	float *tcs = 0; // don't have texture coordinates for now
	if (tcs)
		delete tcs;
	tcs = 0;
	if (yn == 'y') {
		tcs = new float[VertsN * 2];
	}

	ifs.read((char*)Verts, VertsN * 3 * sizeof(float)); // load verts

	if (Colors) {
		ifs.read((char*)Colors, VertsN * 3 * sizeof(float)); // load cols
	}

	if (Normals)
		ifs.read((char*)Normals, VertsN * 3 * sizeof(float)); // load normals

	if (tcs)
		ifs.read((char*)tcs, VertsN * 2 * sizeof(float)); // load texture coordinates

	ifs.read((char*)&TrisN, sizeof(int));
	if (Tris)
		delete Tris;
	Tris = new unsigned int[TrisN * 3];
	ifs.read((char*)Tris, TrisN * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	cerr << "INFO: loaded " << VertsN << " verts, " << TrisN << " tris from " << endl << "      " << filename << endl;
	cerr << "      xyz " << ((Colors) ? "rgb " : "") << ((Normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;
	RecalculateBoundingBox();
}

V3 TMesh::GetCenter() {
	return BoundingBox.GetCenter();
	V3 ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < VertsN; vi++) {
		ret = ret + Verts[vi];
	}
	ret = ret / (float)VertsN;
	return ret;

}

void TMesh::Translate(V3 tv) {

	for (int vi = 0; vi < VertsN; vi++) {
		Verts[vi] = Verts[vi] + tv;
	}
	RecalculateBoundingBox();
}

void TMesh::SetCenter(V3 center) {

	V3 currCenter = GetCenter();
	Translate(center - currCenter);

}



void TMesh::Rotate(V3 aO, V3 aDir, float theta) {

	for (int vi = 0; vi < VertsN; vi++) {
		Verts[vi] = Verts[vi].RotatePoint(aO, aDir, theta);
	}
	RecalculateBoundingBox();
}
