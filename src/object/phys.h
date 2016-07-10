#ifndef REAPER_OBJECT_PHYS_H
#define REAPER_OBJECT_PHYS_H

#include "os/debug.h"

#include "object/base.h"
#include "object/base_data.h"
#include "object/phys_data.h"
#include "object/forward.h"
#include "phys/accessor.h"
#include "object/collide.h"
#include "object/phys_acc.h"

#include "misc/sequence.h"

#include <functional>

namespace reaper {

class MtxParams;

namespace object {
namespace phys {
	
namespace {
	debug::DebugOutput derr("object::phys");
}

class Turret : public Simulate
{
	const controls::Turret &tc;
	MtxParams &pr;
	current_data::Turret& current;
//        weapon::Turret &weap;
        const TurretInfo &info;
	float max_yaw;
	float max_pitch;
public:
	Turret(const controls::Turret &t,MtxParams &p, 
		current_data::Turret &current,
//                weapon::Turret &weap,
		const TurretInfo &info);
	
	void simulate(double dt);
};

class internal_rudders
{
public:
	float pitch;
	float yaw;
	float skew;
	internal_rudders() :
	pitch(0),yaw(0),skew(0) {}
};

struct DynamicData {
	Vector velocity;
	Vector rot_velocity;

	DynamicData() : velocity(0,0,0), rot_velocity(0,0,0) { }
};

class Ship : public Simulate
{
        const controls::Ship &tc;
	const ShipInfo &info;
	current_data::Ship &current;
	BasicDyn* acc;
	internal_rudders rudders;
	short last_ortho;
public:
	Ship(BasicDyn* a,
	     const controls::Ship &tc,
	     current_data::Ship &current,
	     const ShipInfo &info);

	void simulate(double dt);
        Vector compute_acc(const Vector v);
	float get_afterburner() const;
};

class GroundShip : public Simulate
{
protected:
	SillyData& data;
        const controls::GroundShip &tc;
	const GroundShipInfo &info;
	current_data::GroundShip &current;
	BasicDyn* dyn;
	double& sim_time;
	short last_ortho;
        
public:
	GroundShip(SillyData& d, const controls::GroundShip &tc,
		   current_data::GroundShip &current,
		   const GroundShipInfo &info,
		   BasicDyn* bd, double& sim_time)
	 : data(d), tc(tc), info(info), current(current),
       	   dyn(bd), sim_time(sim_time), last_ortho(0)
       	{ }

	 void simulate(double dt);
         Vector compute_acc(const Vector v,const double& dt);
};





// gfx test
class DummyTurret
{
	const controls::Turret &tc;
	MtxParams &pr;
public:
	DummyTurret(const controls::Turret &t,MtxParams &p) : tc(t), pr(p) {}		
	void simulate(double dt);
};

}
}
}
#endif

