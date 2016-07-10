
#ifndef REAPER_GFX_CLOUD_SIMULATE_H
#define REAPER_GFX_CLOUD_SIMULATE_H

#include "main/types.h"
#include "gfx/gfx_types.h"

#include "misc/utility.h"

namespace reaper {
namespace gfx {
namespace cloud {

const int max_cloud_size = 100000;

class Cells {
	typedef bool field[max_cloud_size];
	field hum[2], act[2], cld[2];
	int w, h, d, size;
	float p_hum, p_act, p_cld;

	int c;

	misc::FakeRand rnd;
	bool valid_offset(int i);

	float f_hum(float x, float y, float z);
	float f_act(float x, float y, float z);
	float f_cld(float x, float y, float z);
public:
	Cells(int w, int h, int d);
	void step();
	bool get(int x, int y, int z) const;
};



class SimCloud
{
	Cells cells;
public:
	SimCloud(int w, int h, int d);

	void simulate();

	int width() const;
	int height() const;
	int depth() const;
	
	float get(int x, int y, int z) const;
};




}
}
}

#endif

