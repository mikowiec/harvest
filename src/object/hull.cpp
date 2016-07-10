
/* $Id: hull.cpp,v 1.10 2003/06/04 17:43:06 pstrand Exp $ */

#include "os/compat.h"

#include "object/base_data.h"
#include "object/hull.h"
#include "object/collide.h"
#include "object/event.h"
#include "os/debug.h"

namespace reaper {
namespace object {
namespace hull {

DamageStatus Standard::damage(const CollisionInfo &ci) 
{
	health -= ci.damage;

	if(health<0) {
		health = 0;
		return Fatality;
	} else {
		return Hull;
	}
}

DamageStatus Shielded::damage(const CollisionInfo &ci) 
{
	if(shield > 0) {
		shield -= ci.damage;
		if(shield < 0) {
			float damage_left = -shield;
			shield = 0;
			return Standard::damage( CollisionInfo(damage_left,0) );
		} else {
			shield_hit = true;
			return Shield;
		}
	} else {
		return Standard::damage(ci);
	}
}

void Shielded::regenerate(float ds) 
{
	if(shield < shield_start) {
		shield += ds;
	}
}

bool Shielded::read_shield_hit() 
{
	bool sh = shield_hit;
	shield_hit = false;
	return sh;
}
}
}
}


