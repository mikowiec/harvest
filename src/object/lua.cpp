
#include "os/compat.h"

#include "os/debug.h"

#include "main/types_io.h"

#include "gfx/abstracts.h"
#include "object/phys_acc.h"
#include "phys/accessor.h"

#include "object/reg.h"

#include "object/event.h"
#include "object/factory.h"

#include "misc/lua.h"

namespace reaper {
namespace object {
namespace {
	reaper::debug::DebugOutput derr("object::Lua", 0);
}

struct LuaInfo
{
	float max_acc;
	float max_vel;
	float mass;
	float damage;

	LuaInfo()
	 : max_acc(1),
	   max_vel(1),
	   mass(1),
	   damage(1)
	{ }
	 
};

class LuaTest : public DynamicBase, public Simulate, public Collide
{
	ID id;
	Matrix mtx;
	float rad;
	event::Events ev;
	BasicDyn* acc;
	gfx::RenderInfo ri;
public:
	LuaTest(MkInfo mk)
	 : id(mk.id), mtx(mk.mtx), rad(25), ri("base", mtx, false)
	{
		acc = basic_accessor_ext_mtx(id, mk.cid, mtx, rad);
		derr << "LuaTest() " << mk << '\n';
	}
	~LuaTest()
	{
		delete acc;
	}

	void initialize() {
		DynamicPtr ptr(this);
		typedef DynPhysWrap<LuaInfo, reaper::phys::DynAccessor> Wrap;
		Wrap* w = new Wrap(id, acc, LuaInfo());
		w->add_sim(this);
		w->add_col(this);
		init_reg(ptr, w);
	}

	event::Events& events() { return ev; }

	const gfx::RenderInfo* render(bool eff) const {
//		derr << "render " << *ri.mtx << '\n';
		return &ri;
	}

	const GetStat& stat() const { return *acc; }
	const GetDyn& dyn() const { return *acc; }

	void collide(CollisionInfo ci)
	{
		derr << "collide: " << ci.damage << ' ' << ci.speed_diff << '\n';
	}

	void simulate(double dt)
	{
//		derr << "simulate: " << dt << " : " << acc->mtx() << " " << mtx << '\n';
	}
};

SillyBase* lua(MkInfo mk)
{
	return new LuaTest(mk);
}

namespace {
	struct Foo {
		Foo() {
			factory::inst().register_object("lua", lua);
		}
	} bar;
}

void lua() { }

}
}

