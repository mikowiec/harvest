#include "os/compat.h"
#include "fbmskeleton.h"
#include <math.h>
#include <iostream>


namespace reaper 
{
namespace gfx
{
namespace mesh
{


void FBMSkeleton::build()
{
	int steps = 0;

	float gmin = 10000.0;
	float gmax = 0.0;

	while(steps < iterations){

		float freq = steps * base_freq;
		Perlin perlin;

		const float omega = pow(lacunarity, -beta*0.5*steps);
		const float freq_param = base_freq*pow(lacunarity,steps);

		for(int iy = 0; iy < y; iy++){

			const float ny = ((float) iy / y) * freq_param;

			for(int ix = 0; ix < x; ix++){
				
				const float nx = ((float) ix / x) * freq_param;

				const float old = hf.get(ix, iy);

				const float scale = 1.0 + (old/max)*comodfac;

				//const float val = (perlin.noise2( nx, ny) + 1.0) * 10.0;

				//cout << perlin.noise2(nx, ny) << "\n";

				const float val = old + (perlin.noise2(nx, ny) + 1.0)*scale*omega;

				if(val < gmin) gmin = val;
				if(val > gmax) gmax = val;

				//cout << "scale: " << scale << "\tomega: " << omega << "\tval: " << val << "\n";
				//cin.get();

	
				hf.set(ix, iy, val);

			}
		}

		steps++;

	}

	float gmax_inv = 1.0 / (gmax - gmin);
	for(int iy = 0; iy < y; ++iy)
		for(int ix = 0; ix < x; ++ix)
			hf.set(ix, iy,   max* ((hf.get(ix,iy) - gmin) * gmax_inv)  );

}

}
}
}
