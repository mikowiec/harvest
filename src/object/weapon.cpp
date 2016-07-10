/*
 * $Author: pstrand $
 * $Date: 2003/06/04 17:43:08 $
 * $Log: weapon.cpp,v $
 * Revision 1.37  2003/06/04 17:43:08  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.36  2002/12/29 17:04:41  pstrand
 * hw/ moved to os/
 *
 * Revision 1.35  2002/09/29 12:35:53  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.34  2002/09/27 18:05:04  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.33  2002/04/09 03:36:31  pstrand
 * eff.ptr koll, shot-parent-id, quadtree-update-update
 *
 * Revision 1.32  2002/03/24 17:38:43  pstrand
 * turretss aims a bit bett
 *
 * Revision 1.31  2002/02/27 18:08:41  pstrand
 * release-0.96
 *
 * Revision 1.30  2002/02/08 11:27:59  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.29  2002/02/07 00:02:54  macke
 * STLPort 4.5.1 Win32-fix + shadow & render reorg
 *
 * Revision 1.28  2002/01/10 22:30:29  peter
 * resursfixar.. mm..
 *
 * Revision 1.27  2001/12/05 14:04:15  peter
 * matrix now in (semi)row-major
 *
 * Revision 1.26  2001/12/02 19:47:47  peter
 * get_row -> get_col
 *
 * Revision 1.25  2001/10/09 01:12:53  macke
 * Ska man ha nån chans mot den nya turret-ai:n så.. :)
 *
 * Revision 1.24  2001/08/27 12:57:59  peter
 * *** empty log message ***
 *
 * Revision 1.23  2001/08/27 12:55:31  peter
 * objektmeck...
 *
 * Revision 1.22  2001/08/20 21:41:26  peter
 * aeh
 *
 * Revision 1.21  2001/08/20 17:09:59  peter
 * obj.ptr & fix...
 *
 * Revision 1.20  2001/08/09 18:11:42  macke
 * Fyllefix.. lite här å där..
 *
 * Revision 1.19  2001/08/06 12:16:40  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 */

#include "os/compat.h"

#include "os/debug.h"

#include <iostream>

#include "main/types_ops.h"
#include "main/types_io.h"
#include "object/weapon.h"
#include "object/object.h"
#include "object/factory.h"
#include "gfx/managers.h"
#include "misc/utility.h"
#include "phys/engine.h"
#include "object/phys_data.h"
#include "res/config.h"
#include "object/event.h"

using ::reaper::gfx::MeshRef;
using ::reaper::gfx::MeshMgr;

namespace reaper {
namespace {
	debug::DebugOutput dout("weapon");

	Matrix randomize_x_y(misc::CenterRand& rnd, const Matrix &m, float angle) {
		Matrix rot_x(angle*rnd(), m.col(1));
		Matrix rot_y(angle*rnd(), m.col(2));
		return (m * rot_x) * rot_y;
	}

}

namespace object {
namespace weapon {

using object::ShotPtr;

Weapon::~Weapon()
{ }


template<class Parent>
class WeaponImpl
 : public Parent
{
	WeaponInfo info;
	float inv_rate_of_fire;
	int use_cannon;
	double last_fired;
protected:
	object::ID id;
public:
	WeaponImpl(const MkInfo& mk) :
	   info(mk.info),
	   inv_rate_of_fire(1 / info.rate_of_fire),
	   use_cannon(0), last_fired(0),
	   id(mk.id)
	{
	}
	template<class T>
	WeaponImpl(const MkInfo& mk, T t)
	 : Parent(t),
	   info(mk.info),
	   inv_rate_of_fire(1 / info.rate_of_fire),
	   use_cannon(0), last_fired(0),
	   id(mk.id)
	{
	}
	virtual ShotBase* create_shot(SimAcc* acc) = 0;

	void fire(SimAcc* acc)
	{
		if (acc->get_sim_time() > last_fired + inv_rate_of_fire) {
			ShotBase* s = create_shot(acc);
			if (s != 0) {
				s->initialize();
				use_cannon = ++use_cannon % info.weaps.size();
				last_fired = acc->get_sim_time();
			}
		}
	}
	Matrix weapon_pos()
	{
		return Matrix(info.weaps[use_cannon]);
	}
};


class Laser : public WeaponImpl<Weapon>
{
	misc::CenterRand rnd;
public:
	Laser(const MkInfo& mk)
	 : WeaponImpl<Weapon>(mk), rnd(-0.5, 0.5)
	{ }
	ShotBase* create_shot(SimAcc* acc)
	{
		Matrix create_pos = randomize_x_y(rnd, acc->get_mtx() * weapon_pos(), 1);
		ShotBase* lp = laser(create_pos, Projectile, id);
		return lp;
	}
};

Weapon* mk_laser(const MkInfo& mk)
{
	return new Laser(mk);
}


Missiles::Missiles(MissileType init) : current(init) { }
void Missiles::select(MissileType t) { current = t; }
MissileType Missiles::get_current() const { return current; }
int Missiles::get_num(MissileType) const { return 0; }

class MissilesImpl : public WeaponImpl<Missiles>
{
public:
	MissilesImpl(const MkInfo& mk)
	 : WeaponImpl<Missiles>(mk, Light_Missile)
	{ }

	ShotBase* create_shot(SimAcc* acc)
	{
		ShotBase* lp;
		Matrix create_pos = acc->get_mtx() * weapon_pos();
		switch (get_current()) {
			case Heavy_Missile : 
				lp = heavy_missile(create_pos, Projectile, id);
				break;
			case Light_Missile : 
				lp = light_missile(create_pos, Projectile, id);
				break;

			default:
				lp = 0;  //Add other MissileTypes later if necessary;
		}
		return lp;
	}
};

Missiles* mk_missiles(const MkInfo& mk)
{
	return new MissilesImpl(mk);
}

class Turret : public WeaponImpl<Weapon>
{
	const gfx::RenderInfo& ri;	
	std::string barrel_name;
	misc::CenterRand rnd;
public:
	Turret(MkInfo mk, const gfx::RenderInfo& rndif)
	 : WeaponImpl<Weapon>(mk),
	   ri(rndif),
	   barrel_name(mk.info["barrel_name"]),
	   rnd(-0.5,0.5, mk.id)
	{
	}
	ShotBase* create_shot(SimAcc* acc)
	{
		Matrix mtx = MeshMgr::get_ref()->get_matrix(ri, barrel_name);
		Matrix create_mtx = randomize_x_y(rnd, acc->get_mtx() * mtx * weapon_pos(), 3);
		ShotBase* lp = laser(create_mtx, Projectile, id);
		return lp;
	}
};

Weapon* mk_turret(const MkInfo& mk, const gfx::RenderInfo& ri)
{
	return new Turret(mk, ri);
}

}
}
}
