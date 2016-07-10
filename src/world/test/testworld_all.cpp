

#include "os/compat.h"

#include <algorithm>
#include <functional>

#include "os/gfx.h"
#include "os/event.h"
#include "os/time.h"
#include "os/debug.h"
#include "misc/font.h"
#include <vector>
#include "main/types.h"
#include "gfx/gfx.h"
#include "gfx/camera.h"
#include "gfx/settings.h"
#include "world/world.h"
#include "os/gl.h"
#include "res/res.h"
#include "res/config.h"
#include "os/gfx.h"
#include "os/profile.h"
#include "misc/parse.h"
#include "object/forward.h"
#include "object/object.h"
#include "object/object_impl.h"
#include "object/factory.h"
#include "object/phys.h"
#include "object/phys_data.h"
#include "misc/sequence.h"
#include "gfx/managers.h"
#include "main/types_io.h"
#include "misc/free.h"
#include "phys/engine.h"
#include "main/types_io.h"
#include "misc/free.h"
#include "phys/engine.h"

#include <iostream>

namespace reaper {
namespace object {
	int get_obj_count();
}
}


#include "misc/test_main.h"

namespace reaper {
namespace object {
	int get_obj_count();
}
}

using namespace reaper;


struct test_if {
	virtual void init() = 0;
	virtual void iter() = 0;
	virtual void run() = 0;
	virtual void report() = 0;
	virtual void next() = 0;
	virtual ~test_if() { }
};

template<class T>
struct test : public test_if {
	int acc;
	os::time::Profiler prof;
	T cur, end;
	world::WorldRef wr;
	test(const std::string& name)
	: acc(0), prof(name), wr(world::World::get_ref())
	{ }
	void iter() { ++cur; }
	void run() {
		prof.start();
		init();
		while (cur != end) {
			iter();
			acc++;
		}
		prof.stop();
	}
	void next() { }
	void report() {
		prof.int_report(std::string(" num: ") + misc::ltos(acc, 5));
	}
};

using namespace world;

struct st_all : public test<st_iterator> {
	st_all() : test<st_iterator>("     st_all") { }
	void init() { cur = wr->begin_st(); end = wr->end_st(); }
};

struct dyn_all : public test<world::dyn_iterator> {
	dyn_all() : test<world::dyn_iterator>("    dyn_all") { }
	void init() { cur = wr->begin_dyn(); end = wr->end_dyn(); }
};

template<class T>
struct st_test : public test<st_iterator> {
	T obj;
	st_test(const std::string& name)
	 : test<st_iterator>(name)
	{ }
	void init() {
		cur = wr->find_st((typename T::Org)obj);
		wr->end_tri();
	}
	void next() {
		obj.next();
	}
};

template<class T>
struct dyn_test : public test<world::dyn_iterator> {
	T obj;
	dyn_test(const std::string& name)
	 : test<world::dyn_iterator>(name)
	{ }
	void init() {
		cur = wr->find_dyn((typename T::Org)obj);
		end = wr->end_dyn();
	}
	void next() {
		obj.next();
	}
};

struct test_cyl : public world::Cylinder {
	typedef world::Cylinder Org;
	test_cyl() : world::Cylinder(Point2D(100, 100), 200) { }
	void next() { p.x += 10; p.y += 10; }
};

struct test_sphere : public world::Sphere {
	typedef world::Sphere Org;
	test_sphere() : world::Sphere(Point(0, 400, 0), 200) { }
	void next() { p += Point(10, 10, 10); }
};

struct test_frustum : public world::Frustum {
	typedef world::Frustum Org;
	test_frustum()
	: world::Frustum(Point(500, 400, 500),
			 Vector(0, 0, -1000),
			 norm(Vector(0.9, 0.7, 0.9)), 120, 130)
	{ }
	void next() { 
		pos_ += Point(100, 0, 100);
		dir_ += Vector(100, 0, 100);
		fov_w -= 1;
		fov_h += 1;
	}
};

struct test_line : public world::Line {
	typedef world::Line Org;
	test_line() : world::Line(Point(-1000, 600, 1000),
				  Point(1000, 100,-1000)) { }
	void next() {
		p1 += Point(10, -50, -103);
		p2 += Point(-20, -5, 150);
	}
};

struct move_ship {
	template<class T>
	void operator()(T ship) const
	{
struct show_count {
	template<class T>
	void operator()(T obj) const {
		if (obj.get_count() != 3)
			derr << obj.get_count() << ' ' << obj->get_id() << ' ' << obj->get_pos() << '\n';
	}
};

		Point rnd(-40 + rand() % 80,
			  -10 + rand() % 20, 
			  -40 + rand() % 80);
		Matrix mtx(ship->get_mtx());
		mtx.pos() += rnd;
		ship->get_physics().set_mtx(mtx);
	}
};

struct show_count {
	template<class T>
	void operator()(T obj) const {
		if (obj.get_count() != 3)
			derr << obj.get_count() << ' ' << obj->get_id() << ' ' << obj->get_pos() << '\n';
	}
};

using misc::for_each;
using misc::seq;
using std::mem_fun;

class Stupid : public object::ObjImpl<object::DynamicBase>
{
	object::phys::ObjectAccessor phys;
public:
	Stupid(const Matrix& mtx)
	 : object::ObjImpl<object::DynamicBase>(object::MkInfo("shipX", mtx))
	 , phys(data, dyn_data, 1, 1, 1, 1)
	{ }

	void collide(const object::CollisionInfo&) { }
	void gen_sound(reaper::sound::Interface&) { }
	gfx::RenderInfo* render(bool) const { return 0; }
	void simulate(double) { }
	void think() { }
	void receive(const reaper::ai::Message&) { }
	void add_waypoint(const Point&) { }
	void del_waypoint(Point&) { }
	const object::phys::ObjectAccessor& get_physics() const {
		return phys;
	}
	object::phys::ObjectAccessor& get_physics() {
		return phys;
	}
};

template<class P, class W>
void insert(W wr, int n)
{
	for (int i = 0; i < n; ++i) {
		Point rnd(-1000 + rand() % 1000,
			  rand() % 700, 
			  -1000 + rand() % 1000);
		wr->add_object(P(new Stupid(Matrix(rnd))));
	}
}



template<class W>
void remove(W wr, int n)
{
	while (n--) {
		int r = rand() % 100;
		st_iterator s = wr->begin_st();
		dyn_iterator d = wr->begin_dyn();
		for (int i = 0; i < r; ++i)
			++s;
		r = rand() % 100;
		for (int i = 0; i < r; ++i)
			++d;
		if (s != wr->end_st())
			wr->erase(s);
		if (d != wr->end_dyn())
			wr->erase(d);
		else
			derr << "end dyn\n";
	}
}



int test_main()
{
	srand(10);
	res::ConfigEnv m("foo");
	reaper::gfx::MeshRef::create();
	world::WorldRef wr = world::World::create();
	wr->load("test_level4");
	os::time::Profiler all;

	srand(42);
//	insert<object::StaticPtr>(wr, 5000);
	derr << "start: " << object::get_obj_count() << '\n';
	insert<object::DynamicPtr>(wr, 5000);
	derr << "5000: " << object::get_obj_count() << '\n';
//	wr->update();
	std::vector<test_if*> tests;
	misc::for_each(misc::seq(tests), misc::delete_it);

	derr << "before: " << object::get_obj_count() << '\n';
	wr->shutdown();
	derr << "after: " << object::get_obj_count() << '\n';
	world::WorldRef::destroy();
	derr << "lost: " << object::get_obj_count() << '\n';
	gfx::MeshRef::destroy();
//	tests.push_back(new st_all());
//	tests.push_back(new st_test<test_cyl>("     st_cyl"));
//	tests.push_back(new st_test<test_sphere>("  st_sphere"));
//	tests.push_back(new st_test<test_frustum>(" st_frustum"));
//	tests.push_back(new st_test<test_line>("    st_line"));
	tests.push_back(new dyn_all());
//	tests.push_back(new dyn_test<test_cyl>("    dyn_cyl"));
//	tests.push_back(new dyn_test<test_sphere>(" dyn_sphere"));
//	tests.push_back(new dyn_test<test_frustum>("dyn_frustum"));
//	tests.push_back(new dyn_test<test_line>("   dyn_line"));

	for (int i = 0; i < 20; i++) {
		all.start();
		wr->update();
		for_each(seq(tests), mem_fun(&test_if::run));
		all.stop();
		for_each(seq(tests), mem_fun(&test_if::next));
//		insert<object::StaticPtr>(wr, 500);
		insert<object::DynamicPtr>(wr, 500);
		remove(wr, 100);
		std::for_each(wr->begin_dyn(), wr->end_dyn(), show_count());
		move_ship mv;
		std::for_each(wr->begin_dyn(), wr->end_dyn(), mv);
	}
	for_each(seq(tests), mem_fun(&test_if::report));
	all.int_report("all");
	misc::for_each(misc::seq(tests), misc::delete_it);

	derr << "before: " << object::get_obj_count() << '\n';
	wr->shutdown();
	derr << "after: " << object::get_obj_count() << '\n';
	world::WorldRef::destroy();
	derr << "lost: " << object::get_obj_count() << '\n';
	gfx::MeshRef::destroy();
	return 0;
}



