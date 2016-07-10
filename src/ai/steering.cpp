#include "os/compat.h"
#include "ai/steering.h"
#include "main/types_ops.h"
#include "main/types_io.h"

namespace reaper{
namespace ai{
namespace steering{

void Steering::set_max_speed(float ms)
{
	max_speed = ms;
}

Vector& Steering::seek(Point& target_pos)
{
	Vector desired_vel = norm(target_pos - pos) * max_speed;
	sf = desired_vel - vel;
	
	return sf;
}

Vector& Steering::flee(Point& target_pos)
{
	Vector desired_vel = norm(pos - target_pos) * max_speed;
	sf = desired_vel - vel;
	
	return sf;
}

Vector& Steering::pursuit(Point& target_pos, Vector& target_vel)
{
	// prediction of time until interception
	float T = length(target_pos - pos) / max_speed;
	
	// prediction of interception point
	Point interception = target_pos + (target_vel*T);
	
	// seek this point
	seek(interception);

	return sf;
}

Vector& Steering::evade(Point& target_pos, Vector& target_vel)
{
	// prediction of time until interception
	float T = length(target_pos - pos) / max_speed;
	
	// prediction of interception point
	Point interception = target_pos + (target_vel*T);
	
	// flee this point
	flee(interception);

	return sf;
}

Vector& Steering::arrive(Point& target_pos, float slowing_dist)
{
	Vector target_dir = target_pos - pos;
	float dist = length(target_dir);

	// speed is dependent of distance to goal
	float speed = std::min(max_speed, max_speed*(dist/slowing_dist));

	Vector desired_vel = (speed / dist) * target_dir; // TODO: fix if dist==0
	sf = desired_vel - vel;
	
	return sf;
}

Vector& contain(Point& min, Point& max)
{


}

Vector& separate(float radius, world::WorldRef wr) {}
Vector& cohere(float radius, world::WorldRef wr) {}
Vector& align(float radius, world::WorldRef wr) {}

Vector& Steering::get_steering()
{
	return sf;
}

}
}
}

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:15 $
 * $Log: steering.cpp,v $
 * Revision 1.4  2002/12/29 17:04:15  pstrand
 * hw/ moved to os/
 *
 * Revision 1.3  2002/03/29 10:25:41  nikha
 * ...
 *
 * Revision 1.2  2002/03/28 23:30:21  nikha
 * ...
 *
*/
