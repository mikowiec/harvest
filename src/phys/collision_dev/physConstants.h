#ifndef REAPER_PHYS_CONSTANTS
#define REAPER_PHYS_CONSTANTS

namespace reaper{
namespace phys_dev{
	
	const double Collision_Distance = 0.3; //The minimum distance for which a collision is flagged
	const double Maximum_Lower_bound = 20.0; //The maximum number of seconds for which a lower bound is meat
	const double Minimum_Dynamic_step = 0.03;

}
}

#endif
