
#ifndef REAPER_GFX_CLOUD_PARTICLE_H
#define REAPER_GFX_CLOUD_PARTICLE_H

#include "main/types.h"
#include "gfx/gfx_types.h"

#include <deque>

namespace reaper {
namespace gfx {
namespace cloud {


struct Particle
{
	Point pos;
	float radius;
	std::deque<Color> colors;

	Particle(const Point& p, float r);
};



}
}
}

#endif

