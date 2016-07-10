#ifndef REAPER_GFX_HUD_H
#define REAPER_GFX_HUD_H

#include "object/base.h"
#include "main/enums.h"

namespace reaper {
namespace gfx {
namespace lowlevel {
class HUDImpl;
}

struct HudData
{
	float hull;
	float shield;
	float afterburner;
	MissileType missile;
	int missiles[Number_MissileTypes];
};

class HUD
{
	std::auto_ptr<lowlevel::HUDImpl> i;
public:
	static const float *radar_range;
	static const int *hud_type;

	HUD();
	~HUD();
//	void setup_projection(); ///< Pushes current matrices to stack and sets up orthographic projection	
	void render(const HudData& hud);
};

}
}

#endif

