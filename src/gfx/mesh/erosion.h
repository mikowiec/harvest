#ifndef EROSION_H
#define EROSION_H

#include "shared.h"
#include <math.h>


namespace reaper 
{
namespace gfx
{
namespace mesh
{

class Erosion 
{
private:
	HeightField& hf;
	float max_x, max_y, max_z;

	float sfz, sfx, sfy;
	float diagdif_inv, ydif_inv, xdif_inv;

public:
	Erosion(HeightField& hfi, float max_xi, float max_yi, float max_zi):
		hf(hfi),
		max_x(max_xi),
		max_y(max_yi),
		max_z(max_zi),
		sfx(max_xi / hfi.get_width()),
		sfy(max_yi / hfi.get_height()),
		sfz(max_zi / 1.0),
		diagdif_inv( 1.0 / sqrt(sfx*sfx + sfy*sfy)),
		xdif_inv( 1.0 / sfx),
		ydif_inv( 1.0 / sfy)
	{ /*
			cout << hfi.get_height() << ", " << hf.get_height() << endl;
			cout << hfi.get(20, 20) << ", " << hf.get(20, 20) << endl;
			cout << diagdif_inv << endl;
			cout << xdif_inv << endl;
			cout << ydif_inv << endl;
		//NOP
		*/
	};

	void thermal(float talus, float ct, int iter);
};

}
}
}

#endif
