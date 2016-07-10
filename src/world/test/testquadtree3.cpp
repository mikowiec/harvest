
#include "os/compat.h"
#include "os/debug.h"

#include "main/types.h"
#include "world/triangle.h"
#include "world/world.h"


#include "misc/test_main.h"


using namespace reaper;

class Tri : public world::Triangle
{
	Point p1, p2, p3;
public:
	Tri(const Point2D& pp1, const Point2D& pp2)
	 : world::Triangle(p1, p2, p3),
	   p1(pp1.x, 0, pp1.y),
	   p2(pp1.x, 0, pp2.y),
	   p3(pp2.x, 0, pp2.y)
	{ }
};

int test_main()
{
	quadtree::QuadTree<world::Triangle*> qt(0, 0, 100);

	Point2D p1(10, 10), p2(20, 20), p3(80,20), p4(70, 70), p5(80, 80);

	Tri* t1 = new Tri(p1, p2);
	derr << "insert: " << t1 << '\n';
	qt.insert(t1);
	Tri* t2 = new Tri(p1, p2);
	derr << "insert: " << t2 << '\n';
	qt.insert(t2);
	Tri* t3 = new Tri(p1, p3);
	derr << "insert: " << t3 << '\n';
	qt.insert(t3);
	Tri* t4 = new Tri(p4, p5);
	derr << "insert: " << t4 << '\n';
	qt.insert(t4);



	world::tri_iterator b, e = qt.end();
	int count = 0;
	float add = 0;
	for (b = qt.begin(); b != e; ++b) {
		derr << "got: " << b.deref().unbox() << '\n';
		derr << b->a.x << ' ' <<  b->a.z << ' '
		     << b->b.x << ' ' <<  b->b.z << '\n';
		add += b->a.z + b->b.x;
		count++;
	}
	if (count != 4 || add != 200) {
		derr << "count " << count << " add " << add << "\n";
		return 1;
	}
	derr << "-----------\n";
	count = 0;
	add = 0;
	for (b = qt.find(world::Cylinder(Point2D(55, 25), 25)); b != e; ++b) {
		derr << b->a.x << ' ' <<  b->a.z << ' '
		     << b->c.x << ' ' <<  b->c.z << '\n';
		count++;
		add += b->c.x;
	}
	if (count != 1 || add != 80) {
		derr << "count " << count << " add " << add << "\n";
		return 2;
	}
	derr << "-----------\n";
	b = qt.find(world::Cylinder(Point2D(55, 25), 25));
	qt.erase(b);
	for (b = qt.begin(); b != e; ++b) {
		derr << b->a.x << ' ' <<  b->a.z << ' '
		     << b->b.x << ' ' <<  b->b.z << '\n';
		add += b->a.z + b->b.x;
		count++;
	}


	return 0;
}

