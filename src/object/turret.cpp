
#include "os/compat.h"

#include "main/types_param.h"
#include "object/base.h"
#include "object/factory.h"
#include "object/hull.h"
#include "object/ai.h"
#include "object/renderer.h"
#include "res/config.h"
#include "main/types_io.h"
#include "msg/event.h"
#include "object/weapon.h"
#include "object/collide.h"
#include "object/controls.h"
#include "object/current_data.h"
#include "object/phys.h"
#include "gfx/managers.h"
#include "object/reg.h"

#include "object/object_impl.h"

namespace reaper {
namespace object {


class Turret : public ObjImpl<StaticBase>, public Collide
{
public:
	const TurretInfo phys_data;
protected:
	hull::Standard hull;
	MtxParams ps;

	current_data::Turret current;
	controls::Turret ctrl;
	gfx::RenderInfo ri;
	weapon::Weapon* weap;

	ai::Turret ai;
	phys::Turret phys;	        

	std::auto_ptr<BasicDyn> acc;
public: 
	Turret(MkInfo mk) :
	   ObjImpl<StaticBase>(mk),
	   phys_data(mk.info),
	   hull(phys_data.health), 
	   ps(2), 
	   current(0.0, 0.0), 
	   ri(mk.name, data.get_mtx(), ps, false),
	   weap(weapon::mk_turret(mk, ri)),
	   ai(data, ctrl, current, gfx::MeshMgr::get_ref()->get_matrix(ri, "turret_ball").pos()), 
	   phys(ctrl, ps, current, phys_data)
	{
	}
	~Turret()
	{
		delete weap;
	}

	void initialize()
	{
		StaticPtr ptr(this);
		events().remove.add_listener( reg(&ai) );
		typedef SimPhysWrap<reaper::phys::SimAccessor> Wrap;
		acc = std::auto_ptr<BasicDyn>(basic_accessor(data));
		Wrap* w = new Wrap(data.get_id(), acc.get());
		w->add_sim(&phys);
		w->add_col(this);
		ctrl.fire.set(misc::mk_cmd(weap, &weapon::Weapon::fire,
			static_cast<reaper::phys::SimAccessor*>(w)));
		init_reg(ptr, w);
	}

	void collide(CollisionInfo ci)
	{
		hull::DamageStatus ds = hull.damage(ci);
		damage(ds, ci, data.get_pos());
		if (ds == hull::Fatality)
			events().kill.ping_all();
	}
	const gfx::RenderInfo* render(bool effects) const { return &ri; }
	void simulate(double dt) { }
//	void simulate(double dt)                       { phys.simulate(dt); }
};



void turret() { }
SillyBase* turret(MkInfo mk) { return new Turret(mk); }
namespace {
struct Foo {
	Foo() { factory::inst().register_object("turret", turret); }
} foo;
}
}
}
