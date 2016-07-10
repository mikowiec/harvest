
#include "os/compat.h"

#include "os/debug.h"

#include "main/types_ops.h"
#include "misc/command.h"
#include "misc/free.h"
#include "object/event.h"
#include "object/projectile.h"
#include "phys/accessor.h"
#include "snd/sound_system.h"

namespace reaper {
namespace object {

namespace { debug::DebugOutput derr("projectile"); }


class ShotPhys : public reaper::phys::ShotAccessor
{
	ShotBase* ptr;
	SillyData& data;
	Vector velocity;
	double sim_time;
	double death_time;
	float lifespan;
	float damage;
	ID id;
	ID parent;
public:
	ShotPhys(ShotBase* p, SillyData& d, float speed, double lf, float dmg,
		 ID i, ID pr, Vector v)
	 : ptr(p), data(d), velocity(v),
	   sim_time(0), death_time(sim_time + lf),
	   lifespan(lf), damage(dmg), id(i), parent(pr)
	{ }

	object::ID get_id() const { return id; }
	Matrix get_mtx() const { return data.get_mtx(); }
	float get_radius() const { return ptr->stat().radius(); }
	void collide(float d, float s) {
		ptr->events().kill.ping_all();
	}
	float get_lifespan() const { return lifespan; }
	object::ID get_parent() const { return parent; }
	Vector get_velocity() const { return velocity; }
	float get_damage() const { return damage; }

	void set_sim_time(float t) {
		sim_time = t;
		death_time = sim_time + lifespan;
	}
	float get_sim_time() const { return sim_time; }

	void simulate(float dt) {
		Matrix m(data.get_mtx());
		m.pos() += velocity * dt;
		data.set_mtx(m);
		sim_time += dt;
		if (sim_time > death_time) {
			ptr->events().kill.ping_all();
		}
	}
};

ProjectileBase::ProjectileBase(const std::string& name, const Matrix &mtx,
			       CompanyID c, ID p)
 : data(mtx, c, name, 0.2f, world::alloc_id()),
   phys_data(factory::inst().info(name)),
   velocity(data.get_mtx()*Vector(0,0, -phys_data.speed)),
   sacc(new StatAcc(data)),
   sound(0),
   hull(0.0, 0.0)  //A dummy hull with zero life
{
	acc = new ShotPhys(this, data, phys_data.speed, phys_data.lifespan,
			   phys_data.damage, data.get_id(), p, velocity);
}

ProjectileBase::~ProjectileBase()
{
}


void ProjectileBase::initialize()
{
	ShotPtr ptr(this);
	init_reg(ptr, acc);
}

void ProjectileBase::move()
{
	if (sound) {
		sound->set_location(data.get_mtx());
		sound->set_velocity(velocity);
	}
}

Laser::Laser(const Matrix &mtx, CompanyID c, ID p) : 
	ProjectileBase("laser", mtx, c, p)
{
	sound = sound::create_proj(phys_data.sound, mtx, velocity);
	events().death.add_listener(misc::mk_cmd(this, &Laser::on_kill));
	rd = std::auto_ptr<renderer::Laser>(new renderer::Laser(phys_data, data.get_mtx()));
}

const gfx::RenderInfo* Laser::render(bool effects) const
{
	if (rd.get())
		return rd->render(effects);
	else 
		return 0;
}

Missile::Missile(const std::string& id, const Matrix &mtx, CompanyID c, ID p) : 
ProjectileBase(id, mtx, c, p)
{
	//	sound = sound::create_proj("missile_fly", mtx, velocity);
	if (reaper::sound::Manager::get_ref().valid())
		reaper::sound::Manager::get_ref()->play(phys_data.sound, data.get_pos());

	events().death.add_listener(misc::mk_cmd(this, &Missile::on_kill));
	rd = std::auto_ptr<renderer::Missile>(new renderer::Missile(id, phys_data, data.get_mtx(), data.get_radius()));
}


void Laser::on_kill()
{
	hull_hit(data.get_pos(), phys_data.exp_intensity);
	gfx::static_light(data.get_pos(), phys_data.color, phys_data.light_radius,2).insert_release();
	misc::zero_delete(sound);
}

void Missile::on_kill()
{
	hull_hit(data.get_pos(), phys_data.exp_intensity);
	gfx::static_light(data.get_pos() + Vector(0,5,0), phys_data.color,
			  phys_data.light_radius, .3).insert_release();
	rd->kill();
	misc::zero_delete(sound);
}

const gfx::RenderInfo* Missile::render(bool effects) const
{
	return (rd.get()) ? rd->render(effects) : 0;
}


}
}

