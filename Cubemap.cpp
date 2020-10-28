#include "stdafx.h"
#include "Cubemap.h"

void Cubemap::LoadTiffs(std::string px, std::string nx, std::string py, std::string ny, std::string pz, std::string nz)
{
	TIFF* in0 = TIFFOpen(px.c_str(), "r");
	TIFF* in1 = TIFFOpen(nx.c_str(), "r");
	TIFF* in2 = TIFFOpen(py.c_str(), "r");
	TIFF* in3 = TIFFOpen(ny.c_str(), "r");
	TIFF* in4 = TIFFOpen(pz.c_str(), "r");
	TIFF* in5 = TIFFOpen(nz.c_str(), "r");
	if (in0 == nullptr)
	{
		cerr << px << " could not be opened" << endl;
		return;
	}
	if (in1 == nullptr)
	{
		cerr << nx << " could not be opened" << endl;
		return;
	}
	if (in2 == nullptr)
	{
		cerr << py << " could not be opened" << endl;
		return;
	}
	if (in3 == nullptr)
	{
		cerr << ny << " could not be opened" << endl;
		return;
	}
	if (in4 == nullptr)
	{
		cerr << pz << " could not be opened" << endl;
		return;
	}
	if (in5 == nullptr)
	{
		cerr << nz << " could not be opened" << endl;
		return;
	}
	int resolution;
	int compare;
	TIFFGetField(in0, TIFFTAG_IMAGEWIDTH, &resolution);
	TIFFGetField(in0, TIFFTAG_IMAGELENGTH, &compare);
	if (resolution != compare) return;
	TIFFGetField(in1, TIFFTAG_IMAGEWIDTH, &compare);
	if (resolution != compare) return;
	TIFFGetField(in1, TIFFTAG_IMAGELENGTH, &compare);
	if (resolution != compare) return;
	TIFFGetField(in2, TIFFTAG_IMAGEWIDTH, &compare);
	if (resolution != compare) return;
	TIFFGetField(in2, TIFFTAG_IMAGELENGTH, &compare);
	if (resolution != compare) return;
	TIFFGetField(in3, TIFFTAG_IMAGEWIDTH, &compare);
	if (resolution != compare) return;
	TIFFGetField(in3, TIFFTAG_IMAGELENGTH, &compare);
	if (resolution != compare) return;
	TIFFGetField(in4, TIFFTAG_IMAGEWIDTH, &compare);
	if (resolution != compare) return;
	TIFFGetField(in4, TIFFTAG_IMAGELENGTH, &compare);
	if (resolution != compare) return;
	TIFFGetField(in5, TIFFTAG_IMAGEWIDTH, &compare);
	if (resolution != compare) return;
	TIFFGetField(in5, TIFFTAG_IMAGELENGTH, &compare);
	if (resolution != compare) return;
	if (_Resolution != resolution)
	{
		Reallocate(resolution);
	}
	if (TIFFReadRGBAImage(in0, resolution, resolution, _Data[0].data(), 0) == 0)
	{
		cerr << "failed to load " << px << endl;
	}
	if (TIFFReadRGBAImage(in1, resolution, resolution, _Data[1].data(), 0) == 0)
	{
		cerr << "failed to load " << nx << endl;
	}
	if (TIFFReadRGBAImage(in2, resolution, resolution, _Data[2].data(), 0) == 0)
	{
		cerr << "failed to load " << py << endl;
	}
	if (TIFFReadRGBAImage(in3, resolution, resolution, _Data[3].data(), 0) == 0)
	{
		cerr << "failed to load " << ny << endl;
	}
	if (TIFFReadRGBAImage(in4, resolution, resolution, _Data[4].data(), 0) == 0)
	{
		cerr << "failed to load " << pz << endl;
	}
	if (TIFFReadRGBAImage(in5, resolution, resolution, _Data[5].data(), 0) == 0)
	{
		cerr << "failed to load " << nz << endl;
	}
	TIFFClose(in0);
	TIFFClose(in1);
	TIFFClose(in2);
	TIFFClose(in3);
	TIFFClose(in4);
	TIFFClose(in5);
}
