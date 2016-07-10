#ifndef REAPER_AI_STEERING_H
#define REAPER_AI_STEERING_H

#include "main/types.h"
#include "world/worldref.h"

namespace reaper{
namespace ai{

/// Implementation of various steering behaviours for AI objects
/// All steering functions returns a single steering force vector
namespace steering{

class Steering
{
	Point&  pos;     // ref to objects position
	Vector& vel;     // ref to objects velocity
	float max_speed; // objects max speed

	Vector lx, ly, lz; // local coordinat system

	Vector sf; // desired steering force
public:
	Steering(Point& p, Vector& v, float ms=1.0)
	 : pos(p), vel(v), max_speed(ms)
	{};
	
	void set_max_speed(float ms);
	
	Vector& seek(Point& target_pos);
	Vector& flee(Point& target_pos);
	Vector& pursuit(Point& target_pos, Vector& target_vel);
	Vector& evade(Point& target_pos, Vector& target_vel);
	Vector& arrive(Point& target_pos, float slowing_dist);
	Vector& contain(Point& min, Point& max);
	Vector& separate(float radius, world::WorldRef wr);
	Vector& cohere(float radius, world::WorldRef wr);
	Vector& align(float radius, world::WorldRef wr);

	Vector& get_steering();
};

}
}
}

#endif

/*
 * $Author: nikha $
 * $Date: 2002/03/29 10:25:41 $
 * $Log: steering.h,v $
 * Revision 1.3  2002/03/29 10:25:41  nikha
 * ...
 *
 * Revision 1.2  2002/03/28 23:30:21  nikha
 * ...
 *
*/
