#ifndef REAPER_OBJECT_PHYS_DATA_H
#define REAPER_OBJECT_PHYS_DATA_H

#include <string>
#include <iosfwd>

#include "main/types.h"
#include "gfx/gfx_types.h"
#include "misc/parse.h"

namespace reaper {
namespace res { class ConfigEnv; }
namespace object {

/// Holds all physics info for all objects of the same type

class ShotInfo
{
public:
	const float speed;
	const float lifespan;
	const float damage;
	const float trail_size;
	const Point engine_pos;
	const gfx::Color color;
	const std::string sound;
	float exp_intensity;
	float light_radius;
	ShotInfo(const std::string& m,
		const float speed,
		const float lifespan,
		const float damage,
		const float trail,
		const Point& pos,
		const gfx::Color& col,
		const std::string& snd,
		float ei,
		float lr)
	 : speed(speed),
	   lifespan(lifespan),
	   damage(damage),
	   trail_size(trail),
	   engine_pos(pos),
	   color(col),
	   sound(snd),
	   exp_intensity(ei),
	   light_radius(lr)
	{ }
	ShotInfo(const res::ConfigEnv& env);
};

class DummyInfo
{
        const float strength;
        DummyInfo(const float st) :
        strength(st) {}
};

class WeaponInfo
{
public:
        float rate_of_fire;
        std::vector<Vector> weaps;
        WeaponInfo(const float rof, const std::vector<Vector>& weaps)
	 : rate_of_fire(rof), weaps(weaps)
        { }
	WeaponInfo(const res::ConfigEnv& env);
};

class TurretInfo
{
public:
	const float health;
	const float shield;
	const float shield_recharge;
	const float damage;
        const float max_yaw;
        const float max_pitch;
	TurretInfo(const float health,
		const float shield,
		const float shield_recharge,
		const float damage,
                const float max_yaw,
                const float max_pitch):
		health(health),
		shield(shield),
		shield_recharge(shield_recharge),
		damage(damage),
                max_yaw(max_yaw),
                max_pitch(max_pitch) {}
	TurretInfo(const res::ConfigEnv& env);
};

class CommonShipInfo {
public:
	float min_thrust;
	float max_thrust;
	float mass;
	float max_acc;
        float max_vel;
	float drag_coeff;
	float max_rot_y;
	float damage;
	float health;
	float shield;
	float shield_recharge;

	CommonShipInfo( float min_thrust,
			float max_thrust,
			float mass,
			float max_acc,
			float mvel,
			float drag_coeff,
			float max_rot_y,
			float damage,
			float health,
			float shield,
			float shield_recharge)
	 :	min_thrust(min_thrust),
		max_thrust(max_thrust),
		mass(mass),
		max_acc(max_acc),
                max_vel(mvel),
		drag_coeff(drag_coeff),
		max_rot_y(max_rot_y),
		damage(damage),
		health(health),
		shield(shield),
		shield_recharge(shield_recharge)
	{ }
	CommonShipInfo(const res::ConfigEnv& env);
	void dump(std::ostream&) const;
};


class GroundShipInfo : public CommonShipInfo {
public:
	const float altitude;

	GroundShipInfo(const float min_thrust,
		const float max_thrust,
		const float mass,
		const float max_acc,
                const float mvel,
		const float drag_coeff,
		const float max_rot_y,
		const float damage,
		const float health,
		const float shield,
		const float shield_recharge,
                const float alt) :	
	CommonShipInfo( min_thrust,
			max_thrust,
			mass,
			max_acc,
			mvel,
			drag_coeff,
			max_rot_y,
			damage,
			health,
			shield,
			shield_recharge),
        altitude(alt)
	{ }
	GroundShipInfo(const res::ConfigEnv& env);
};



class ShipInfo : public GroundShipInfo
{
public:
	const float afterburner;
	const float burn_rate;
	const float ab_recharge_rate;
	const float rudder_drag;
	const float max_rot_x;
	const float max_rot_z;
	const Point engine_pos;
	ShipInfo(	const float min_thrust,
			const float max_thrust,
			const float mass,
			const float max_acc,
			const float mvel,
			const float drag_coeff,
			const float max_rot_y,
			const float damage,
			const float health,
			const float shield,
			const float shield_recharge,
			const float afterburner,
			const float burn_rate,
			const float ab_recharge_rate,
			const float rudder_drag,
			const float max_rot_x,
			const float max_rot_z,
			const Point engine_pos)
	 :	GroundShipInfo( min_thrust, max_thrust, mass,
				max_acc, mvel, drag_coeff, max_rot_y,
				damage, health, shield, shield_recharge, 0),
	 	afterburner(afterburner),
		burn_rate(burn_rate),
		ab_recharge_rate(ab_recharge_rate),
		rudder_drag(rudder_drag),
		max_rot_x(max_rot_x),
		max_rot_z(max_rot_z),
		engine_pos(engine_pos)
	{ }
	ShipInfo(const res::ConfigEnv& env);
	void dump(std::ostream&) const;
};

}
}
#endif

