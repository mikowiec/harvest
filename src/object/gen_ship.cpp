
#include "os/compat.h"

#include "os/debug.h"

#include "object/gen_ship.h"
#include "object/sound.h"
#include "object/weapon.h"

namespace reaper {
namespace object {

namespace {
	debug::DebugOutput derr("gen_ship");
}



class notify_death
{
	renderer::Ship &r;
	sound::Ship* s;
public:
	notify_death(renderer::Ship &rr, sound::Ship* ss) : r(rr), s(ss) {}
	void operator()() {
		r.kill();
		s->kill();
	}
};

typedef DynPhysWrap<ShipInfo, reaper::phys::DynAccessor> Wrap;

GenShip::GenShip(MkInfo mk) :
	ObjImpl<PlayerBase>(mk),
	acc(basic_accessor(data)),
	phys_data(mk.info),
	hull(phys_data.health, phys_data.shield),
	phys(acc, ctrl, current, phys_data),
	rd(mk.name, data.get_mtx(), data.get_radius(), data.get_id(), hull, 1, phys_data.engine_pos), // trail size = 1
	snd(sound::create_ship(mk.info["real_name"], data.get_mtx(), acc->velocity())), 
	laser(weapon::mk_laser(mk)),
	missiles(weapon::mk_missiles(mk)),
	phys_acc(new Wrap(data.get_id(), acc, phys_data))
{
	ctrl.current_missile.onchange(misc::mk_cmd(missiles, &weapon::Missiles::select));
	current.eng.thrust.onchange(misc::mk_cmd(snd, &sound::Ship::engine_pitch));
	events().death.add_listener(misc::mk_cmd(notify_death(rd, snd)));
	events().remove.add_listener(misc::mk_cmd(snd, &sound::Ship::kill));
	set_weapon(&ctrl.fire, laser, (reaper::phys::SimAccessor*)phys_acc);
	set_weapon(&ctrl.missile_fire, missiles, (reaper::phys::SimAccessor*)phys_acc);
}

GenShip::~GenShip()
{
	delete laser;
	delete missiles;
	delete snd;
	delete acc;
}


void GenShip::initialize()
{
	phys_acc->add_sim(&phys);
	phys_acc->add_sim(this);
	phys_acc->add_col(this);
	DynamicPtr ptr(this);
	init_reg(ptr, phys_acc);
}

const gfx::RenderInfo* GenShip::render(bool effects) const
{
	rd.render_hud(hull, phys, phys_data, ctrl.current_missile, missiles);
	return rd.render(effects);
}


void GenShip::collide(CollisionInfo ci) 
{
	hull::DamageStatus ds = hull.damage(ci);
	air_damage(ds,ci,data.get_pos(),acc->velocity());
	switch (ds) {
	case hull::Shield:
		rd.shield_hit(ci);
		break;
	case hull::Fatality:
		data.events->kill.ping_all();
		break;
	default: break;
	}
	
}

void GenShip::simulate(double dt) 
{
//	phys.simulate(dt);
	hull.regenerate(dt*phys_data.shield_recharge);
	rd.simulate(dt);
	snd->set_location(acc->mtx());
}

}
}

