
#include "os/compat.h"

#include <vector>

#include "os/debug.h"
#include "world/world.h"
#include "gfx/gfx.h"
#include "main/types.h"
#include "main/types_io.h"
#include "object/base.h"
#include "object/phys.h"
#include "misc/smartptr.h"
#include "misc/test_main.h"

using namespace reaper;
using namespace reaper::world;
using namespace reaper::object;

class TestShot
 : public ShotBase
{
public:
	TestShot(int i)
	 : ShotBase(Matrix(Vector(10*i,2,1)), 1.0, Nature, 1.0, 1.0, Vector(1,1,1)) { }
	void simulate(double) { }
	void render() const { }
	void collide(const CollisionInfo&) { }
	float get_damage() const { return 1; }
};

class TestDyn
: public DynamicBase
{
	object::phys::ObjectAccessor phys_access;
public:
	TestDyn(int i) : DynamicBase(Matrix(Vector(20*i,1,2)), 1.0, Nature, 1.0)
	 , phys_access(data, dyn_data, 1, 1, 1, 1)
	{ }
	void think() { }
	void simulate(double) { }
	void render() const { }
	virtual void receive(const ::reaper::ai::Message &) { }
	void collide(const CollisionInfo&) { }
	float get_damage() const { return 1; }
	virtual void add_waypoint(const Point& p) { };
	virtual void del_waypoint(Point& p) { };
	virtual       object::phys::ObjectAccessor& get_physics() { return phys_access; }
	virtual const object::phys::ObjectAccessor& get_physics() const { return phys_access; }
};


int test_main()
{
	debug::ExitFail ef(1);

	WorldRef w = World::create("test_level");

	for (int i = 0; i < 10; ++i) {
		w->add_object(object::ShotPtr(new TestShot(i)));
		w->add_object(object::DynamicPtr(new TestDyn(i)));
	}

	int i = 0;
	{
		shot_iterator c, e = w->end_shot();

		for (c = w->begin_shot(); c != e; ++c, ++i) {
			derr << "shot: " << c->get_pos() << '\n';
		}
		for (c = w->find_shot(Cylinder(Point2D(55, 0), 18)); c != e; ++c, ++i) {
			derr << "shot (cyl): " << c->get_pos() << '\n';
		}
		for (c = w->find_shot(Sphere(Point(55, 0, 0), 18)); c != e; ++c, ++i) {
			derr << "shot (sph): " << c->get_pos() << '\n';
		}
		for (c = w->find_shot(Frustum(Point(55, 0, 0), Vector(20, 0, 0), Vector(0, 1, 0), 50, 50));
		     c != e; ++c, ++i) {
			derr << "shot (fru): " << c->get_pos() << '\n';
		}
	}
	{
		dyn_iterator c, e = w->end_dyn();
		for (c = w->begin_dyn(); c != e; ++c, ++i) {
			derr << "dyn: " << c->get_pos() << '\n';
		}
	}

	if (i == 20)
		ef.disable();
	return 0;
}

