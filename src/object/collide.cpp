
/* $id: $ */

#include "os/compat.h"

#include "main/types_ops.h"
#include "gfx/gfx.h"
#include "gfx/gfx_types.h"
#include "gfx/instances.h"
#include "snd/sound_system.h"
#include "os/debug.h"

#include "object/collide.h"



namespace reaper {
namespace object {

namespace
{
using namespace reaper;
using namespace reaper::object::hull;
using reaper::object::CollisionInfo;
using gfx::Color;

reaper::debug::DebugOutput dout("object::collide",0);
void silly_explosion(const Point &pos)
{
	gfx::ground_explosion(pos,10).insert_release();
	gfx::grey_smoke(pos,3).insert_release();
	gfx::smokepuff(pos,10).insert_release();
	gfx::static_light(pos+Vector(0,30,0),Color(1,.5,.1),200,.5).insert_release();
	reaper::sound::Manager::get_ref()->play("explosion_large", pos, 1.0);
}

void air_explosion(const Point &pos, const Vector &vel)
{
	gfx::air_explosion(pos,vel,4).insert_release();
	gfx::static_light(pos+Vector(0,30,0),Color(1,.5,.1),200,.3).insert_release();
	hull_hit(pos,20);	
	reaper::sound::Manager::get_ref()->play("explosion_larger", pos, 1.0);
}
} // end anonymous namespace

void hull_hit(const Point &pos, float i)
{
	gfx::smokepuff(pos,1+i/2).insert_release();
	gfx::sparks(pos,1+i/2).insert_release();
	reaper::sound::Manager::get_ref()->play("projectile_hit", pos, 1.0);
}

void damage(DamageStatus ds, const CollisionInfo &ci, const Point &pos)
{
	switch(ds) {
	case Hull:     hull_hit(pos, ci.damage); break;
	case Fatality: silly_explosion(pos); break;
	default: break;
	}
}

void air_damage(DamageStatus ds, const CollisionInfo &ci, 
		const Point &pos, const Vector &vel)
{
	switch(ds) {
	case Hull:     hull_hit(pos, ci.damage); break;
	case Fatality: air_explosion(pos, vel);  break;
	default: break;
	}

}


/*
void HeavyMissile::collide(const CollisionInfo&)
{
	hull_hit(get_pos(),0);
	gfx::static_light(get_pos()+Vector(0,5,0),Color(1,.4,0),30,1).insert_release();
	kill();
	rd.kill();
//	snd.kill();
}
*/

/*
void HeavyMissile::collide(const CollisionInfo&)
{
	hull_hit(get_pos(),0);
	gfx::static_light(get_pos()+Vector(0,5,0),Color(1,.4,0),30,1).insert_release();
	kill();
	rd.kill();
//	snd.kill();
}
*/


}
}

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:40 $
 * $Log: collide.cpp,v $
 * Revision 1.35  2002/12/29 17:04:40  pstrand
 * hw/ moved to os/
 *
 * Revision 1.34  2002/04/14 09:22:37  pstrand
 * quadtree redesign and Sphere::radius -> Sphere::r
 *
 * Revision 1.33  2002/01/26 23:27:14  macke
 * Matrox fix, plus some misc stuff..
 *
 * Revision 1.32  2002/01/23 01:00:02  peter
 * soundfixes again
 *
 * Revision 1.31  2002/01/21 20:56:46  peter
 * soundtweaking...
 *
 * Revision 1.30  2001/08/27 12:55:27  peter
 * objektmeck...
 *
 * Revision 1.29  2001/08/20 17:09:57  peter
 * obj.ptr & fix...
 *
 * Revision 1.28  2001/08/06 12:16:35  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.27.2.1  2001/08/05 14:01:30  peter
 * objektmeck...
 *
 * Revision 1.27  2001/07/30 23:43:27  macke
 * Häpp, då var det kört.
 *
 * Revision 1.26  2001/07/24 23:52:49  macke
 * Jo, explicit ska det vara (fel på annat ställe)..  rättade till lite array-fel också..
 *
 * Revision 1.25  2001/07/23 23:48:10  macke
 * Slimmad grafikhantering samt lite namnbyten
 *
 * Revision 1.24  2001/07/06 01:47:30  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.23  2001/06/09 01:58:57  macke
 * Grafikmotor reorg
 *
 * Revision 1.22  2001/05/25 09:54:46  peter
 * ljudfixar..
 *
 * Revision 1.21  2001/05/25 09:17:10  peter
 * no message
 *
 */

