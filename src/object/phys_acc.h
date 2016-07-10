
#ifndef REAPER_OBJECT_PHYS_ACC_H
#define REAPER_OBJECT_PHYS_ACC_H


#include "object/accessor.h"
#include "object/collide.h"



#include <vector>


namespace reaper {
namespace object {


struct Simulate
{
	virtual void simulate(double delta) = 0;
};

struct Collide
{
	virtual void collide(CollisionInfo) = 0;
};

template<class Base>
class PrimPhysWrap : public Base
{
	ID id;
	std::vector<Collide*> cols;

	GetStat* acc;
public:
	PrimPhysWrap(ID i, GetStat* a)
	 : id(i), acc(a)
	{ }

	void add_col(Collide* c) {
		cols.push_back(c);
	}

	/* Prim */
	object::ID get_id() const { return id; }
	Matrix get_mtx()    const { return acc->mtx(); }
	float get_radius()  const { return acc->radius(); }
	void collide(float d, float s) {
		misc::for_each(misc::seq(cols),
				std::bind2nd(std::mem_fun(&Collide::collide), 
				object::CollisionInfo(d, s)));
	}
};

template<class Base>
class SimPhysWrap : public PrimPhysWrap<Base>
{
	double sim_time;
	std::vector<Simulate*> sims;

public:
	SimPhysWrap(ID i, GetStat* a)
	 : PrimPhysWrap<Base>(i, a),
	   sim_time(0)
	{ }

	void add_sim(Simulate* s) {
		sims.push_back(s);
	}

	/* Sim */
	float get_sim_time() const { return sim_time; }
	void set_sim_time(float s) { sim_time = s; }
	void simulate(float delta) {
		misc::for_each(misc::seq(sims),
				std::bind2nd(std::mem_fun(&Simulate::simulate), delta));
		sim_time += delta;
	}
};

template<class Info, class Base>
class DynPhysWrap : public SimPhysWrap<Base>
{
	Info info;

	BasicDyn* acc;
public:
	DynPhysWrap(ID i, BasicDyn* a, Info inf)
	 : SimPhysWrap<Base>(i, a), info(inf), acc(a)
	{ }

	void set_mtx(const Matrix& m) {
		acc->set_mtx(m);
	}

	virtual Vector get_velocity() const {
		return acc->velocity();
	}
	void set_velocity(const Vector& v) {
		acc->set_velocity(v);
	}
	virtual Vector get_rotvel() const {
		return acc->rot_velocity();
	}
	void set_rotvel(const Vector& v) {
		acc->set_rot_velocity(v);
	}

	virtual float max_acc() const {
		return info.max_acc;
	}
	virtual float max_vel() const {
		return info.max_vel;
	}
	virtual float get_mass() const {
		return info.mass;
	}
	virtual float get_damage() const {
		return info.damage;
	}
};

}
}

#endif

