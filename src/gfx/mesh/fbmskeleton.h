#ifndef FBMSKELETON_H
#define FBMSKELETON_H

#include <time.h>

#include "shared.h"
#include "perlin.h"


namespace reaper 
{
namespace gfx
{
namespace mesh
{


class FBMSkeleton{
private:
	
	void build();

public:

	int iterations;
	float max;
	int x, y;
	float lacunarity;
	float beta;
	float comodfac;		// Crossoverscale-modulation factor
	float base_freq;

	HeightField hf;

	FBMSkeleton(int ix, int iy, float imax, float ibase_freq, int iiterations = 3, float ibeta = 1.0, float icomodfac = 1.0, float ilacunarity = 2.0):
		x(ix),
		y(iy),
		max(imax),
		base_freq(ibase_freq),
		comodfac(icomodfac),
		lacunarity(ilacunarity),
		beta(ibeta),
		hf(ix, iy),
		iterations(iiterations)
	{
		build();
	};

	void randomize(){srand((unsigned)time(NULL));}

};

}
}
}

#endif
