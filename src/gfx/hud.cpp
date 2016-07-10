#include "os/compat.h"

#include <sstream>

#include "main/types_ops.h"
#include "main/enums.h"

#include "gfx/gfx.h"
#include "gfx/misc.h"
#include "gfx/settings.h"
#include "gfx/exceptions.h"
#include "gfx/displaylist.h"

#include "gfx/interfaces.h"
#include "gfx/managers.h"
#include "gfx/hud.h"

#include "object/base.h"
#include "object/phys.h"
#include "object/accessor.h"
#include "world/world.h"
#include "os/gl.h"

#include "misc/utility.h"
#include "misc/parse.h"
#include "misc/font.h"

using namespace reaper::gfx::misc;

namespace reaper {
namespace {

debug::DebugOutput dout("gfx::HUD",0);
const TexCoord tex_a(0,0), tex_b(1,0), tex_c(1,1), tex_d(0,1);

inline void draw_meter(float x, float y, float val)
{
	glColor3f(0,0,1);
	meter(x + .12, y, .3, .016, 1, 1);

	if(val < .25) {
		glColor3f(1,0,0);
	} else if( val < .75) {
		glColor3f(1,(val-.25)*2,0);
	} else {
		glColor3f(1-(val-.75)*4,1,0);
	}
	meter(x + .125, y + .002, .29, .013, 1, val);
}

inline void draw_meter_text(float x, float y, float val, const std::string &desc)
{
	std::ostringstream ss;
	ss << desc << int(val*100) << "%";
	gfx::font::glPutStr(x, y, ss.str());
}

std::vector<Point> lines;

template<class It>
inline void radar_blips(It i, const It &end, float y0)
{	

	while(i != end) {
		if(((*i)->stat().company() != Nature)) {
			Point p = (*i)->stat().pos();
			lines.push_back(Point(p.x, p.z, 0));
			lines.push_back(Point(p.x, p.z, 2*(y0-p.y)));
		}
		++i;
	}

	if(!lines.empty()) {
		glVertexPointer(3, GL_FLOAT, sizeof(Point), &lines[0]);
		glDrawArrays(GL_LINES, 0, lines.size());
		glVertexPointer(3, GL_FLOAT, sizeof(Point)*2, &lines[0]);
		glDrawArrays(GL_POINTS, 0, lines.size()/2);
		lines.clear();
	}
}

} // end anonymous namespace

namespace gfx {
const float *HUD::radar_range = &Settings::current.radar_range;
const int *HUD::hud_type  = &Settings::current.hud_type;

namespace lowlevel {

class HUDImpl
{
	misc::DisplayList crosshair, missiles,
		          light_missile, heavy_missile,
			  target_missile;

	Point  pos;
	Vector dir;

	world::WorldRef wr;
	TextureRef tr;
	object::DynamicWPtr lp;

	inline void draw_radar();
	inline void draw_speedometer();
	inline void draw_status(float, float, float); // hull, shield, afterburner
	inline void draw_missiles(MissileType type, const int num[]);
public:
	HUDImpl();
	~HUDImpl() {}

	void render(const HudData&);
};

inline void HUDImpl::draw_radar()
{
	const world::Cylinder c(Point2D(pos.x,pos.z), *HUD::radar_range);

	glColor3f(0,1,0);
	font::glPutStr(.87,.735, reaper::misc::ltos(itrunc(*HUD::radar_range)), font::Small);

	glPushMatrix();
	glLoadIdentity();
	glTranslatef(.875,.85,0);
	glScalef(0.075,.1,1);

	glRotatef(30, 1, 0, 0);

	// draw background
	tr->use("hud_radar", 1);
	glColor4f(1,1,1,.5);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord(tex_a); glVertex2f(-1, 1);
	glTexCoord(tex_b); glVertex2f( 1, 1);
	glTexCoord(tex_d); glVertex2f(-1,-1);
	glTexCoord(tex_c); glVertex2f( 1,-1);
	glEnd();

	// draw blips and lines
	const float inv_r = 0.9f / *HUD::radar_range;

	tr->use("");
	glPointSize(3);
	glScalef(-inv_r, inv_r, inv_r);

	Vector dir = lp->stat().mtx().col(2);
	float angle = atan2(dir.z, dir.x) * 180/3.14;
	glRotatef(-90-angle, 0, 0, 1);

	glTranslatef(-pos.x, -pos.z, 0);
	glColor3f(0,0,1); radar_blips(wr->find_si(c),  wr->end_si(),  pos.y);
	glColor3f(0,1,0); radar_blips(wr->find_st(c),  wr->end_st(),  pos.y);
	glColor3f(1,0,0); radar_blips(wr->find_dyn(c), wr->end_dyn(), pos.y);

	glPopMatrix();
}

inline void HUDImpl::draw_speedometer()
{
	const float speed = length(lp->dyn().velocity());
	const float max_vel = 300; // FIXME: get from object database.
	const float norm_speed = (speed / max_vel) * .16;

	tr->use("");
	glBegin(GL_QUADS);
		glColor3f(0,1,0);
		glVertex2f(.02,.04);
		glVertex2f(.04,.04);
		glColor4f(0,.3,0,.5);
		glVertex2f(.04,.2);
		glVertex2f(.03,.2);

		glVertex2f(.07,.2);
		glVertex2f(.06,.2);
		glColor3f(0,1,0);
		glVertex2f(.06,.04);
		glVertex2f(.08,.04);

		glColor3f(0,5*(.04+norm_speed),0);
		glVertex2f(.04,.04);
		glVertex2f(.06,.04);
		glColor3f(0,.5,0);
		glVertex2f(.06,.04 + norm_speed);
		glVertex2f(.04,.04 + norm_speed);
	glEnd();

	glColor3f(1,0,1);
	std::ostringstream ss;
 	ss.setf(std::ios::fixed);
 	ss.precision(0);	
	ss << speed * 3.6 << " kph";	
	font::glPutStr(0.02,0.01,ss.str().c_str());
}

inline void HUDImpl::draw_status(float hull, float shield, float afterburner)
{
	tr->use("");
	draw_meter(.01,.97, hull);
	draw_meter(.01,.95, shield);
	draw_meter(.01,.93, afterburner);
	glColor3f(1,1,1);
	draw_meter_text(.01,.97, hull, "Hull:   ");
	draw_meter_text(.01,.95, shield, "Shield: ");
	draw_meter_text(.01,.93, afterburner, "Boost:  ");
}

inline void HUDImpl::draw_missiles(MissileType type, const int num[])
{
	using gfx::font::glPutStr;
	using reaper::misc::ltos;

	const float x = .69;
	const float y = .16;
	const float w = .3;
	const float h = .15;
	const float tex_w = 206;
	const float tex_h = 63;	

	tr->use("hud_missiles", 1);
	glColor4f(1,1,1,1);
	missiles.call();	

	tr->use("hud_missiles_active", 1);
	glPushMatrix();
	glTranslatef(x,y,0);
	glScalef(w/tex_w, -h/tex_h,1);

	switch (type) {
	case Light_Missile:  light_missile.call(); break; 
	case Target_Missile: target_missile.call(); break;
	case Heavy_Missile:  heavy_missile.call(); break;
	default: break;
	}

	if (num == 0) {
		glColor4f(1,0,0,1);
	} else {
		glColor4f(0,1,0,1);
	}

	glPopMatrix();

	glColor4f(0,1,1,1);
	/*
	glPutStr(0.743, 0.069, ltos(lp->get_missiles(Light_Missile)),  font::Small,0.012,0.017);
	glPutStr(0.842, 0.069, ltos(lp->get_missiles(Target_Missile)), font::Small,0.012,0.017);
	glPutStr(0.941, 0.069, ltos(lp->get_missiles(Heavy_Missile)),  font::Small,0.012,0.017);
	*/
	glPutStr(0.742, 0.070, "inf",  font::Medium,0.011,0.016);
	glPutStr(0.844, 0.070, "0",    font::Medium,0.011,0.016);
	glPutStr(0.940, 0.070, "inf",  font::Medium,0.011,0.016);
}

void HUDImpl::render(const HudData& h)
{
	pos = lp->stat().pos();
	dir = norm(lp->stat().mtx().col(2));

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	// setup matrices
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0,1,0,1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	if (*HUD::hud_type == Internal_HUD) {
		tr->use("hud_target_area", 1);
		crosshair.call();
	}
	draw_radar();
	draw_missiles(h.missile, h.missiles);
	draw_speedometer();
	draw_status(h.hull, h.shield, h.afterburner);

	// restore matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	if (*HUD::hud_type == External_HUD) {
		tr->use("hud_target_area", 1);

		glEnable(GL_DEPTH_TEST);
		glColor3f(1,1,1);
		glEnable(GL_BLEND);

		BillBoard::add(1, pos-dir*60);
		BillBoard::add(1, pos-dir*35);
		BillBoard::render();

		glDisable(GL_DEPTH_TEST);
	}
}


HUDImpl::HUDImpl() 
: tr(TextureMgr::get_ref()),
  lp(wr->get_local_player().get_weak_ptr())
{
	crosshair.begin();
	glColor3f(1,1,1);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord(tex_a); glVertex2f(.35,.65);
	glTexCoord(tex_b); glVertex2f(.65,.65);
	glTexCoord(tex_d); glVertex2f(.35,.35);
	glTexCoord(tex_c); glVertex2f(.65,.35);
	glEnd();
	crosshair.end();

	missiles.begin();
	glColor3f(1,1,1);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0,1);         glVertex2f(.69,.01);
	glTexCoord2f(206/256.0,1); glVertex2f(.99,.01);
	glTexCoord2f(0,0);         glVertex2f(.69,.16);
	glTexCoord2f(206/256.0,0); glVertex2f(.99,.16);
	glEnd();
	missiles.end();

	light_missile.begin();
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0,1);        glVertex2f(0,63);
	glTexCoord2f(70/256.0,1); glVertex2f(70,63);
	glTexCoord2f(0,0);        glVertex2f(0,0);
	glTexCoord2f(70/256.0,0); glVertex2f(70,0);
	glEnd();
	light_missile.end();

	target_missile.begin();
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(70/256.0,1);  glVertex2f(70,63);
	glTexCoord2f(136/256.0,1); glVertex2f(136,63);
	glTexCoord2f(70/256.0,0);  glVertex2f(70,0);
	glTexCoord2f(136/256.0,0); glVertex2f(136,0);
	glEnd();
	target_missile.end();

	heavy_missile.begin();
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(136/256.0,1); glVertex2f(136,63);
	glTexCoord2f(206/256.0,1); glVertex2f(206,63);
	glTexCoord2f(136/256.0,0); glVertex2f(136,0);
	glTexCoord2f(206/256.0,0); glVertex2f(206,0);
	glEnd();
	heavy_missile.end();
}

} // end lowlevel namespace

HUD::HUD() : i(new lowlevel::HUDImpl) { }
HUD::~HUD() { }
void HUD::render(const HudData& h) { i->render(h); }

}
}

/*
 * $Author: pstrand $
 * $Date: 2003/06/11 15:16:42 $
 * $Log: hud.cpp,v $
 * Revision 1.50  2003/06/11 15:16:42  pstrand
 * småfixar..
 *
 * Revision 1.49  2003/06/04 16:29:58  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.48  2002/12/29 17:04:33  pstrand
 * hw/ moved to os/
 *
 * Revision 1.47  2002/09/23 19:52:18  pstrand
 * non-win32-workaround not needed anymore...
 *
 * Revision 1.46  2002/09/23 19:39:34  pstrand
 * *** empty log message ***
 *
 * Revision 1.45  2002/09/20 23:46:59  pstrand
 * use_texturing->texture_level, minor event fix.
 *
 * Revision 1.44  2002/09/13 07:45:55  pstrand
 * gcc-varningar
 *
 * Revision 1.43  2002/09/08 16:55:58  fizzgig
 * cleanup
 *
 * Revision 1.42  2002/05/21 10:04:30  pstrand
 * world.iter.
 *
 * Revision 1.41  2002/05/10 14:12:50  fizzgig
 * shadow code cleanup
 *
 * Revision 1.40  2002/03/24 17:35:51  pstrand
 * to_str -> ltos
 */
