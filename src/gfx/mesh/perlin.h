#ifndef PERLIN_H
#define PERLIN_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


namespace reaper 
{
namespace gfx
{
namespace mesh
{


class Perlin {
private:

	const int B; //B-value
	const int BM;
	const int N;
	const int NP;
	const int NM;

	int* p;
	float* g1;
	float* g2;

	int start;

	void init();
	void normalize2(float v[2]);
	
public:

	Perlin():
		B(0x100),
		BM(0xff),
		N(0x1000),
		NP(12),
		NM(0xfff),
		start(1)
	{
		p = new int[B+B+2];

		g2 = new float[2*(B+B+2)];
		g1 = new float[B+B+2];
	};

	~Perlin()
	{
		delete [] p;
		delete [] g1;
		delete [] g2;
	};

	double noise1(double arg);
	float noise2(float vec[2]);
	float noise2(float x, float y);

};

}
}
}

#endif
