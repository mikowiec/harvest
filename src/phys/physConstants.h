#ifndef REAPER_PHYS_CONSTANTS
#define REAPER_PHYS_CONSTANTS

namespace reaper{
namespace phys{
	
	const double Collision_Distance = 0.8; //The minimum distance for which a collision is flagged
	const double Maximum_Lower_bound = 20.0; //The maximum number of seconds for which a lower bound is meat
	const double Maximum_Dynamic_speed = 500;
	const double Maximum_Shot_speed = 1500;
	const double Minimum_Static_step = 3*(Collision_Distance*0.9)/Maximum_Dynamic_speed;
	const double Minimum_Dynamic_step = 3*Minimum_Static_step*0.5;
	const double Minimum_Shot_step = 10*Collision_Distance*0.9/Maximum_Shot_speed;

}
}

#endif

