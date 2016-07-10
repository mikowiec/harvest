
#include "os/compat.h"

#include <iostream>

#include "res/config.h"
#include "object/phys_data.h"
#include "gfx/io.h"
#include "main/types_io.h"


namespace reaper {
namespace object {

ShotInfo::ShotInfo(const res::ConfigEnv& env)
 : speed(env["speed"]),
   lifespan(env["lifetime"]),
   damage(env["damage"]),
   trail_size(env["trail_size"]),
   engine_pos(read<Point>(env["engine_pos"])),
   color(read<gfx::Color>(env["color"])),
   sound(env["sound"]),
   exp_intensity(env["exp_intensity"]),
   light_radius(env["light_radius"])
{ }

WeaponInfo::WeaponInfo(const res::ConfigEnv& env)
 : rate_of_fire(env["rate_of_fire"])
{
	std::vector<Vector> w_pos;
	int c = 1;
	do {
		std::string var("pos" + misc::ltos(c));
		if (! env.defined(var))
			break;
		Vector v(read<Vector>(env[var]));
		weaps.push_back(read<Vector>(env[var]));
		++c;
	} while (true);
}

TurretInfo::TurretInfo(const res::ConfigEnv& env)
 : health(env["health"]),
   shield(env["shield"]),
   shield_recharge(env["shield_recharge"]),
   damage(env["damage"]),
   max_yaw(env["max_rate_of_yaw"]),
   max_pitch(env["max_rate_of_pitch"])
{ }

CommonShipInfo::CommonShipInfo(const res::ConfigEnv& env)
 :  	min_thrust(env["min_thrust"]),
	max_thrust(env["max_thrust"]),
	mass(env["mass"]),
	max_acc(env["max_accel"]),
	max_vel(env["max_vel"]),
	drag_coeff(env["drag"]),
	max_rot_y(env["max_yaw_rate"]),
	damage(env["damage_mult"]),
	health(env["health"]),
	shield(env["shield"]),
	shield_recharge(env["shield_recharge"])
{ }

void CommonShipInfo::dump(std::ostream& os) const
{
	misc::dumper(os)("min_thrust", min_thrust)
			("max_thrust", max_thrust)
			("mass", mass)
			("max_accel", max_acc)
			("max_vel", max_vel)
			("drag", drag_coeff)
			("max_yaw_rate", max_rot_y)
			("damage_mult", damage)
			("health", health)
			("shield", shield)
			("shield_recharge", shield_recharge);

}

GroundShipInfo::GroundShipInfo(const res::ConfigEnv& env)
 : CommonShipInfo(env), altitude(env["altitude"])
{ }

ShipInfo::ShipInfo(const res::ConfigEnv& env)
 : 	GroundShipInfo(env),
	afterburner(env["afterburner"]),
	burn_rate(env["burn_rate"]),
	ab_recharge_rate(env["afterburner_recharge"]),
	rudder_drag(env["rudder_drag"]),
	max_rot_x(env["max_pitch_rate"]),
	max_rot_z(env["max_roll_rate"]),
	engine_pos(read<Point>(env["engine_pos"]))
{ }

void ShipInfo::dump(std::ostream& os) const
{
	CommonShipInfo::dump(os);
	misc::dumper(os)("afterburner", afterburner)
			("burn_rate", burn_rate)
			("afterburner_recharge", ab_recharge_rate)
			("rudder_drag", rudder_drag)
			("max_pitch_rate", max_rot_x)
			("max_roll_rate", max_rot_z)
			("engine_pos", engine_pos);
			
}

}
}

