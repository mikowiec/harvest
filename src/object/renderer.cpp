/*
 * $Author: pstrand $
 * $Date: 2003/06/04 17:43:07 $
 * $Log: renderer.cpp,v $
 * Revision 1.50  2003/06/04 17:43:07  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.49  2002/12/29 17:04:40  pstrand
 * hw/ moved to os/
 *
 * Revision 1.48  2002/09/23 12:08:44  fizzgig
 * Moved all anonymous namespaces into the reaper-namespace
 *
 * Revision 1.47  2002/02/08 11:27:58  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.46  2002/02/07 00:02:54  macke
 * STLPort 4.5.1 Win32-fix + shadow & render reorg
 *
 * Revision 1.45  2002/01/21 20:56:47  peter
 * soundtweaking...
 */

#include "os/compat.h"

#include "gfx/gfx.h"
#include "object/object_impl.h"
#include "os/debug.h"
#include "gfx/hud.h"
#include "object/weapon.h"
#include "object/phys.h"
#include "object/phys_data.h"
#include "snd/sound_system.h"

#include "main/types_io.h"

namespace reaper {
namespace {
	reaper::debug::DebugOutput dout("object::renderer");
}
namespace object {


using gfx::Color;

namespace renderer
{

/////////////////////////////////////////////////////////////////////////////////

Shield::Shield(const Matrix &m, const float &r, hull::Shielded &hull) : 
	h(hull), mtx(m), color(0,0,1,0), sphere(gfx::sphere(m, r, color))
{}

void Shield::render() const {
	if(color.a > 0) {
		sphere.draw();
	}
}
void Shield::simulate(float dt) {
	if(color.a > 0) {
		color.a -= dt;
	}
}
void Shield::shield_hit(const CollisionInfo & ci) {
	reaper::sound::Manager::get_ref()->play("own_ship_hit", mtx.pos());
	color.a = .7;
}


////////////////////////////////////////////////////////////////////////////

Laser::Laser(const ShotInfo& si, const Matrix& mtx)
 : shot(gfx::shot(mtx, si.color)),
   light(0) //new effects::DynamicLight(mtx, Point(0, 0, 0), col, 20, 0, false))
{ }

class HudRenderer : public gfx::Hud
{
	Ship* ship;
	friend class Ship;
	gfx::HudData data;
public:
	HudRenderer(Ship* s) : ship(s) { }
	void render() {
		if (ship && ship->hud)
			ship->hud->render(data);
	}
};

Ship::Ship(const std::string &mesh, const Matrix &mtx, float radius, ID id,
	   hull::Shielded &h, float trail_size, const Point &engine_pos) :
	hull(mesh, mtx, false), shield(mtx, radius, h), 
	trail(gfx::enginetrail(mtx, trail_size, engine_pos)),
	hud(0), hud_rnd(0)
{
	hud_rnd = new HudRenderer(this);
	gfx::hud_reg(id, hud_rnd);
	trail.insert();
}

Ship::~Ship()
{
	delete hud;
	hud_rnd->ship = 0;
}

void Ship::render_hud(const hull::Shielded& s,
		      const phys::Ship& phys, const ShipInfo& info,
		      MissileType mt, const weapon::Missiles* m) const
{
	if (hud == 0)
		hud = new gfx::HUD();
	gfx::HudData& h = hud_rnd->data;
	h.hull = s.get_health() / info.health;
	h.shield = s.get_shield() / info.shield;
	h.afterburner = phys.get_afterburner() / info.afterburner;
	h.missile = mt;
	h.missiles[Light_Missile] = m->get_num(Light_Missile);
	h.missiles[Heavy_Missile] = m->get_num(Heavy_Missile);
	h.missiles[Target_Missile] = m->get_num(Target_Missile);
}

Missile::Missile(const std::string& mesh, const ShotInfo& si, const Matrix &mtx, float radius) :
	hull(mesh, mtx, false),
	trail(gfx::smoketrail(mtx, si.trail_size, si.engine_pos))
{
	trail.insert();
}

}
}
}
