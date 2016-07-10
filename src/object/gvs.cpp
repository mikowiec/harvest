

#include "os/compat.h"

#include <string>

#include "ai/waypoints.h"
#include "ai/thinker.h"
#include "object/reg.h"
#include "object/base.h"
#include "object/hull.h"
#include "res/config.h"
#include "object/weapon.h"
#include "object/collide.h"
#include "object/current_data.h"
#include "object/controls.h"
#include "object/renderer.h"
#include "object/ai.h"
#include "object/phys.h"
#include "object/object_impl.h"
#include "object/phys_data.h"
#include "main/types_param.h"
#include "main/types_io.h"
#include "object/base_data.h"
#include "object/accessor.h"
#include "object/phys_acc.h"

namespace reaper {
namespace object {


class GroundShip : public DynamicBase, public Collide
{
public:
	ShipInfo phys_data;
protected:
	double sim_time;
	SillyData data;
        hull::Shielded hull;		
	current_data::GroundShip current;
	
	controls::GroundShip ctrl;

	BasicDyn* dyni;
	phys::GroundShip phys;

public:	
	GroundShip(MkInfo);

	void initialize();

	event::Events& events() { return *data.events; }

	const GetStat& stat() const { return *dyni; }
	const GetDyn& dyn() const { return *dyni; }

	void collide(CollisionInfo cI);
};

class GroundOrdinary : public GroundShip
{
	ai::GVOrdinary ai;
	renderer::Ship rd;
public:
	GroundOrdinary(MkInfo);

	void initialize();
	void think(void)                   { ai.think(); }
	void erase_waypoints(void)	   { ai.erase_waypoints(); }
	const gfx::RenderInfo* render(bool effects) const { return rd.render(effects); }
	void simulate(double dt);  
};

class GroundContainer
	:public GroundShip
{
	ai::GVContainer ai;
	renderer::Ship rd;
public:
	GroundContainer(MkInfo);

	void initialize();
	void think(void)                  { ai.think(); }
	void erase_waypoints(void)	  { ai.erase_waypoints(); }
	const gfx::RenderInfo* render(bool effects) const { return rd.render(effects); }
	void simulate(double dt);  
};

class GroundTurret
 : public GroundShip
{
	const TurretInfo& phys_turret_data;
public:
	MtxParams ps;
	controls::Turret ctrl_turret;	
	current_data::Turret current_turret;
	gfx::RenderInfo ri;

	weapon::Weapon* weap_turret;
	
	ai::GVTurret ai;
	phys::Turret phys_turret;	
        
public:
	GroundTurret(MkInfo);

	void initialize();
	double get_sim_time() { return sim_time; }
	void set_sim_time(double s) { sim_time = s; }

	void simulate(double dt);
	const gfx::RenderInfo* render(bool effects) const { return &ri; }
	void think()				      { ai.think(); }
	void erase_waypoints(void)	              { ai.erase_waypoints(); }
};

void GroundOrdinary::initialize() {
	events().remove.add_listener( reg(&ai) );
	GroundShip::initialize();
}

void GroundContainer::initialize() {
	events().remove.add_listener( reg(&ai) );
	GroundShip::initialize();
}

void GroundTurret::initialize() {
	events().remove.add_listener( reg(&ai) );
	GroundShip::initialize();
}

GroundShip::GroundShip(MkInfo mk)
 : phys_data(mk.info),
   data(mk.mtx, mk.cid, mk.name, mk.id), 
   hull(phys_data.health, phys_data.shield),
   dyni(basic_accessor(data)),
   phys(data, ctrl, current, phys_data, dyni, sim_time)
{ }


typedef DynPhysWrap<ShipInfo, reaper::phys::DynAccessor> Wrap;


void GroundShip::initialize()
{
	DynamicPtr ptr(this);
	Wrap* phys_acc = new Wrap(data.get_id(), dyni, phys_data);
	phys_acc->add_sim(&phys);
	phys_acc->add_col(this);
	init_reg(ptr, phys_acc);
}

GroundOrdinary::GroundOrdinary(MkInfo mk) :
	GroundShip(mk),
	ai(mk, mk.cid, dyni, ctrl),
	rd(mk.name, data.get_mtx(), data.get_radius(), mk.id, hull, 0, Point(0, .2, .2))
{
	reaper::ai::add_waypoints(&ai, mk.cdata);
}

GroundContainer::GroundContainer(MkInfo mk) :
	GroundShip(mk),
	ai(mk, mk.cid, dyni, ctrl),
	rd(mk.name, data.get_mtx(), data.get_radius(), mk.id, hull, 0, Point(0, .2, .2))
{
	reaper::ai::add_waypoints(&ai, mk.cdata);
}

GroundTurret::GroundTurret(MkInfo mk) :
	GroundShip(mk),
	phys_turret_data(mk.info),
	ps(2), 
	current_turret(0.0, 0.0), 
	ri(mk.name, data.get_mtx(), ps, false),
//	weap_turret(data.m, weap_data, mesh_id, ps, sim_time,"ground-turret_ball_barrel"), 
	weap_turret(weapon::mk_turret(mk, ri)),
	ai(mk, mk.cid, dyni, ctrl, ctrl_turret, current_turret), 
	phys_turret(ctrl_turret, ps, current_turret, phys_turret_data)
{
	reaper::ai::add_waypoints(&ai, mk.cdata);
//	debug::std_debug << "gt init: " << mtx.get_pos() << " set: " << trk.mtx.get_pos() << '\n';
}

void GroundShip::collide(CollisionInfo ci) 
{

	hull::DamageStatus ds = hull.damage(ci);
	air_damage(ds, ci, data.get_pos(), dyni->velocity());
	if (ds == hull::Fatality)
		data.events->kill.ping_all();
}
        void GroundOrdinary::simulate(double dt)
        {
//		printf("pos: %f %f %f\n", get_pos().x, get_pos().y, get_pos().z);
                phys.simulate(dt);
		hull.regenerate(dt*phys_data.shield_recharge);
		rd.simulate(dt);
        }

	void GroundContainer::simulate(double dt)
        {
                phys.simulate(dt);
		hull.regenerate(dt*phys_data.shield_recharge);
		rd.simulate(dt);
        }

	void GroundTurret::simulate(double dt)
	{	
		phys.simulate(dt);
		double sim = get_sim_time();
		phys_turret.simulate(dt); // FIXME, had sim_time. fix..
		set_sim_time(sim);
		hull.regenerate(dt*phys_data.shield_recharge);
	}



namespace {

SillyBase* ground_ordinary(MkInfo mk)  { return new GroundOrdinary(mk); }
SillyBase* ground_container(MkInfo mk) { return new GroundContainer(mk); }
SillyBase* ground_turret(MkInfo mk)    { return new GroundTurret(mk); }

struct Foo {
Foo() {
	factory::inst().register_object("ground-container", ground_container);
	factory::inst().register_object("ground-vehicle", ground_ordinary);
	factory::inst().register_object("ground-turret", ground_turret);
}
} bar;

} // end anonymous namespace


void gvs() { }

}
}

