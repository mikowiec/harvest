
/* $Id: collide.h,v 1.1 2001/08/27 12:55:27 peter Exp $ */

#ifndef REAPER_OBJECT_COLLIDE_H
#define REAPER_OBJECT_COLLIDE_H

#include "object/hull.h"

namespace reaper {

class Point;
class Vector;

namespace object {

class CollisionInfo
{
public:
	const float damage;
	const float speed_diff;
	CollisionInfo(const float d,const float spdif) : damage(d), speed_diff(spdif) {}
};		

void hull_hit(const Point &pos, float i);

void damage(hull::DamageStatus ds, const CollisionInfo &ci, const Point &pos);

void air_damage(hull::DamageStatus ds, const CollisionInfo &ci, 
		const Point &pos, const Vector &vel);

}
}

#endif
