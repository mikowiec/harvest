
#include "os/compat.h"

#include "gfx/cloud/simulate.h"

#include "os/debug.h"
#include "main/types.h"
#include "gfx/gfx_types.h"
#include "misc/utility.h"

#include <math.h>

namespace reaper {
namespace gfx {
namespace cloud {

namespace { debug::DebugOutput derr("cloud"); }


Cells::Cells(int wi, int hi, int di)
 : w(wi), h(hi), d(di), size(w*h*d),
   p_hum(0.01), p_act(0.0001), p_cld(0.03),
   c(0), rnd(0, 1)
{
	for (int i = 0; i < size; ++i) {
		if (valid_offset(i)) {
			hum[c][i] = rnd() >= 0.98;
			act[c][i] = rnd() >= 0.98;
		}
	}
}

bool Cells::valid_offset(int i)
{
	int w_i = i % w;
	int h_i = (i / w) % h;
	int d_i = i / (w*h);
	return
	   (w_i > 0 && w_i < w &&
	    h_i > 0 && h_i < 3 && 
	    d_i > 0 && d_i < d);
}

float dist(float x, float y, float z, float wi, float hi, float di)
{
	float w = wi/2.0;
	float h = hi/2.0;
	float d = di/2.0;
	return sqrt((x-w)*(x-w) + (y-h)*(y-h) + (z-d)*(z-d));
}

float Cells::f_hum(float x, float y, float z)
{
	return 1.0 - dist(x,y+h/4.0,z,w,h,d)/30.0;
}

float Cells::f_act(float x, float y, float z)
{
	return f_hum(x,y,z);
}

float Cells::f_cld(float x, float y, float z)
{
	return f_hum(x,y,z);
}

void Cells::step()
{
	int n = c^1;
	int i = 0;
	const int yoff = w;
	const int zoff = w*h;

	for (int x = 0; x < w; ++x) {
		for (int y = 0; y < h; ++y) {
			for (int z = 0; z < d; ++z) {
				hum[n][i] = (hum[c][i] && !act[c][i]) || (rnd() < p_hum*f_hum(x,y,z));
				cld[n][i] = (cld[c][i] || act[c][i]) && (rnd() > p_cld*f_cld(x,y,z));

				bool act_env = 
					((z>1)   ? act[c][i - zoff] : false) ||
					((z>2)   ? act[c][i - 2*zoff] : false) ||
					((z<d-1) ? act[c][i + zoff] : false) ||
					((z<d-2) ? act[c][i + 2*zoff] : false) ||
					((x>1)   ? act[c][i - 1] : false) ||
					((x>2)   ? act[c][i - 2] : false) ||
					((x<w-1) ? act[c][i + 1] : false) ||
					((x<w-2) ? act[c][i + 2] : false) ||
					((y>1)   ? act[c][i - yoff] : false) ||
					((y>2)   ? act[c][i - 2*yoff] : false) ||
					((y<h-1) ? act[c][i + yoff] : false) ||
					((y<h-2) ? act[c][i + 2*yoff] : false);
				act[n][i] = (!act[c][i] && hum[c][i] && act_env) || (rnd() < p_act*f_act(x,y,z));

				++i;
			}
		}
	}
	c = n;
}

bool Cells::get(int x, int y, int z) const
{
	if (x<0 || y<0 || z<0 || x>=w || y>=h || z>=d)
		return false;
	int off = x+y*w+z*w*h;
	return cld[c][off];
}

SimCloud::SimCloud(int w, int h, int d)
 : cells(w,h,d)
{
}

void SimCloud::simulate()
{
	cells.step();
}
	
float SimCloud::get(int x, int y, int z) const
{
	bool b = cells.get(x,y,z);
	float f = b ? 0.5 + 0.1 * (cells.get(x-1,y,z)+cells.get(x+1,y,z)+
		         	   cells.get(x,y-1,z)+cells.get(x,y+1,z)+
				   cells.get(x,y,z-1)+cells.get(x,y,z+1))
			: 0.0;
	return f;
}




}
}
}

