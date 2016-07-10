
#include "os/compat.h"

#include "ai/waypoints.h"
#include "object/ai.h"
#include "main/types_io.h"

#include "os/extra.h"

#include <stdio.h>

namespace reaper {
namespace ai {


void add_waypoints(object::ai::WayPointy* o, const res::ConfigEnv& cdata)
{
	char label[25];
	int idx = 1;

	do {
		snprintf(label, 25, "waypoint%d", idx++);
		if (! cdata.defined(label))
			break;
		o->add_waypoint(read<Point>(cdata[label]));
	} while (true);
}


}
}

