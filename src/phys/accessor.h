#ifndef REAPER_PHYS_ACCESSOR_H
#define REAPER_PHYS_ACCESSOR_H

#include "main/types.h"
#include "misc/smartptr.h"

namespace reaper {

namespace phys {
namespace sim {

struct Prim
{
	virtual ~Prim();

	virtual int get_id() const = 0;
	virtual Matrix get_mtx() const = 0;
	virtual float get_radius() const = 0;
	virtual void collide(float damage, float speed_diff) = 0;
};

struct Sim
{
	virtual float get_sim_time() const = 0;
	virtual void set_sim_time(float) = 0;
	virtual void simulate(float) = 0;
};

struct Dyn
{
	virtual void set_mtx(const Matrix&) = 0;

	virtual Vector get_velocity() const = 0;
	virtual void set_velocity(const Vector&) = 0;

	virtual Vector get_rotvel() const = 0;
	virtual void set_rotvel(const Vector&) = 0;

	virtual float max_acc() const = 0;
	virtual float max_vel() const = 0;
	virtual float get_mass() const = 0;
	virtual float get_damage() const = 0;
};

struct Shot
{
	virtual float get_lifespan() const = 0;
	virtual int get_parent() const = 0;
	virtual Vector get_velocity() const = 0;
	virtual float get_damage() const = 0;

};


}

class PrimAccessor : public sim::Prim { };
class SimAccessor  : public sim::Sim,  public PrimAccessor { };
class DynAccessor  : public sim::Dyn,  public SimAccessor { };
class ShotAccessor : public sim::Shot, public SimAccessor { };

typedef misc::SmartPtr<PrimAccessor> PrimAccessorPtr;
typedef misc::SmartPtr<SimAccessor> SimAccessorPtr;
typedef misc::SmartPtr<DynAccessor> DynAccessorPtr;
typedef misc::SmartPtr<ShotAccessor> ShotAccessorPtr;

}
}

#endif

