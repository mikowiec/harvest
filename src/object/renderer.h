#ifndef REAPER_OBJECT_GFX_H
#define REAPER_OBJECT_GFX_H

#include <string>
#include "gfx/instances.h"
#include "object/base.h"
#include "main/enums.h"

#include "object/forward.h"

namespace reaper {
namespace gfx {
	class HUD;
}
namespace object {

namespace renderer {
using namespace ::reaper::gfx;

class Shield
{
	hull::Shielded &h;
	const Matrix &mtx;
	Color color;
	EffectPtr sphere;
public:
	Shield(const Matrix &mtx, const float &radius, hull::Shielded &hull);

	void render() const;
	void simulate(float dt);
	void shield_hit(const CollisionInfo &ci);
};

class Laser
{
	EffectPtr shot;
	SimEffectPtr light;
public:
	Laser(const object::ShotInfo& si, const Matrix &mtx);

	const gfx::RenderInfo* render(bool effects) const { 
		if(effects) {
			shot.draw();
		}
		return 0;
	}
};

class HudRenderer;

class Ship
{
	RenderInfo hull;
	Shield shield;
	SimEffectPtr trail;
	mutable gfx::HUD* hud;
	HudRenderer* hud_rnd;
	friend class HudRenderer;
public:
	Ship(const std::string &mesh, const Matrix &mtx, float radius, 
	      ID id, hull::Shielded &h, float trail_size, const Point &engine_pos);
	~Ship();

	const gfx::RenderInfo* render(bool effects) const {
		if(effects) {
			shield.render();
		}
		return &hull;
	}

	void render_hud(const hull::Shielded&,
			const phys::Ship& phys, const ShipInfo&,
			MissileType mt, 
			const weapon::Missiles*) const;
	void simulate(float dt) { shield.simulate(dt); }
	void kill() { trail.transfer(); }
	void shield_hit(const CollisionInfo & ci) { shield.shield_hit(ci); }
};

class Missile
{
	RenderInfo hull;
	SimEffectPtr trail;
	bool killed;
public:
	Missile(const std::string& mesh, const object::ShotInfo& si,
		const Matrix &mtx, float radius);
	
	const gfx::RenderInfo* render(bool effects) const { return &hull; }
	void kill() { trail.transfer(); }
};
 

}
}
}
#endif

/*
 * $Author: pstrand $
 * $Date: 2003/06/04 17:43:08 $
 * $Log: renderer.h,v $
 * Revision 1.31  2003/06/04 17:43:08  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.30  2002/02/08 11:27:58  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.29  2002/01/10 23:09:14  macke
 * massa bök
 *
 * Revision 1.28  2001/11/10 11:58:34  peter
 * diverse minnesfixar och strul...
 *
 * Revision 1.27  2001/08/27 14:50:16  peter
 * *** empty log message ***
 *
 * Revision 1.26  2001/08/27 12:55:30  peter
 * objektmeck...
 *
 * Revision 1.25  2001/08/20 17:09:59  peter
 * obj.ptr & fix...
 *
 * Revision 1.24  2001/08/18 16:46:22  macke
 * Grafikfixar
 *
 * Revision 1.23  2001/08/06 12:16:39  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.22.2.3  2001/08/05 14:01:33  peter
 * objektmeck...
 *
 * Revision 1.22.2.2  2001/08/04 20:37:08  peter
 * flyttad hud, vapenknôk..
 *
 * Revision 1.22.2.1  2001/08/03 13:44:12  peter
 * pragma once obsolete...
 *
 * Revision 1.22  2001/07/30 23:43:30  macke
 * Häpp, då var det kört.
 *
 * Revision 1.21  2001/07/23 23:48:10  macke
 * Slimmad grafikhantering samt lite namnbyten
 *
 * Revision 1.20  2001/07/06 01:47:33  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.19  2001/06/09 01:58:58  macke
 * Grafikmotor reorg
 *
 * Revision 1.18  2001/05/14 20:00:56  macke
 * bugfix och rök på missiler..
 *
 */

