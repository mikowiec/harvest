
#include "os/compat.h"

#include <iostream>
#include <list>

#include "world/geometry.h"
#include "misc/parse.h"
#include "os/time.h"
#include "misc/test_main.h"

using namespace reaper;

int test_main()
{
	srand(42);
	
	os::time::Profiler prof;
	world::Frustum fru(Point(305, 212, 417), Vector(301, 302, 405),
			   norm(Vector(-0.17, 0.7, -0.16)), 87, 118);
	std::list<world::Triangle> tl;
	int n = 0;
	for (int i = 0; i < 10; ++i) {
		tl.clear();
		for (int j = 0; j < 10000; ++j) {
			Point* a = new Point(rand() % 1000, rand() % 1000, rand() % 1000);
			Point* b = new Point(a->x + rand() % 40, a->y + rand() % 40, a->z + rand() % 40);
			Point* c = new Point(a->x + rand() % 40, a->y + rand() % 40, a->z + rand() % 40);
			
			tl.push_back(world::Triangle(*a, *b, *c));
		}
		std::list<world::Triangle>::iterator c, e = tl.end();
		prof.start();
		for (c = tl.begin(); c != e; ++c) {
			n += world::intersect(*c, fru);
		}
		prof.stop();
	}
	prof.int_report(misc::ltos(n, 5));
	


	return 0;
}

