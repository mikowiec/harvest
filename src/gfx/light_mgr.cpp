#include "os/compat.h"

#include "os/gl.h"
#include "os/debug.h"

#include "gfx/misc.h"
#include "gfx/managers.h"
#include "gfx/exceptions.h"
#include "gfx/light.h"
#include "gfx/io.h"
#include "gfx/settings.h"

#include "world/world.h"
#include "game/level.h"
#include "world/geometry.h"

#include "misc/parse.h"
#include "misc/sequence.h"
#include "misc/free.h"

#include "main/types_io.h"
#include "main/types4_io.h"

#include "res/res.h"
#include "res/config.h"

#include <sstream>
#include <iterator>

namespace reaper {
namespace misc { UniquePtr<gfx::LightMgr>::I UniquePtr<gfx::LightMgr>::inst; }
namespace gfx {
namespace lowlevel {
namespace {
debug::DebugOutput dout("gfx::light_mgr",0);
debug::DebugOutput derr("gfx::light_mgr", 5);
}

using namespace reaper::misc;

using light::Light;

class LightMgrImpl
{

	typedef std::list<Light*>          GlobalCont;
	typedef quadtree::QuadTree<Light*> StaticCont;
	typedef std::list<Light*>          DynamicCont;

	typedef GlobalCont::iterator  global_iterator;
	typedef StaticCont::iterator  static_iterator;
	typedef DynamicCont::iterator dynamic_iterator;

	std::deque<Light*> read_lights;  // light sources that we own

	GlobalCont globals;
	DynamicCont dynamics;
	std::auto_ptr<StaticCont> statics;

	int active_statics;
	int active_globals;
	int active_dynamics;

	const int max_lights;

	int n_statics;

	world::WorldRef wr;
	TextureRef tr;
public:	
	LightMgrImpl();
	~LightMgrImpl(); 
	
	void setup_global();                               ///< Call each frame	
	void setup_locals(const Point& pos, float radius); ///< Call for each object	
	void render(const world::Frustum &f);              ///< Render lightmaps on terrain

	void add_global(Light *l);
	void add_static(Light *l);
	void add_dynamic(Light *l);

	void remove_global(Light *l);
	void remove_static(Light *l);
	void remove_dynamic(Light *l);

	Color calc_diffuse_light(const Point &p, const Vector &n, const Material &m);

	int num_static_lights();
};

LightMgrImpl::LightMgrImpl() :
	active_statics(0), active_globals(0), active_dynamics(0),
	max_lights(glGetInteger(GL_MAX_LIGHTS)),
	n_statics(0),
	tr(gfx::TextureMgr::get_ref())
{
	using namespace std;

	const game::LevelInfo &li = wr->get_level_info();
	statics = std::auto_ptr<StaticCont>(new StaticCont(li.terrain_min_x, li.terrain_min_z, li.terrain_max_x - li.terrain_min_x));

	std::vector<game::LevelInfo::Light>::const_iterator lc, le = li.lights.end();
	for (lc = li.lights.begin(); lc != le; ++lc) {
		Light* light = new Light(
				lc->pos, lc->dir, lc->exponent, lc->angle,
				lc->diffuse, lc->specular, lc->ambient,
				lc->const_att, lc->linear_att, lc->quadratic_att);
		read_lights.push_back(light);

		/* old critera for global:
		 * (pos.w == 0 || amb != Color(0,0,0,1) || (la==0 && qa==0))
		 */
		if (lc->global) {
			globals.push_back(light);
		} else {
			statics->insert(light);
		}
	}		

	//dout << "LightMgrImpl(): Read " << g << " global and " << l << " static lights\n";
}

LightMgrImpl::~LightMgrImpl()
{
	for_each(seq(read_lights), delete_it);
	//dout << "LightMgrImpl(): destroyed\n";
}

void LightMgrImpl::setup_global()
{
	for(int i = 0; i<active_globals + active_statics + active_dynamics; ++i) {
		glDisable(GL_LIGHT0 + i);
	}

	active_globals = 0;

	for(global_iterator i = globals.begin(); i != globals.end(); ++i) {		
		glEnable(GL_LIGHT0 + active_globals);
		(*i)->activate(active_globals);
		++active_globals;
		if(active_globals >= max_lights)
			break;
	}		

	throw_on_gl_error("light::LightMgrImpl::setup_global()");
}

void LightMgrImpl::setup_locals(const Point& pos, float radius)
{
	const int lights = active_statics + active_globals;		
	int c = active_globals;
	active_statics = 0;
	active_dynamics = 0;

	for(static_iterator i = statics->find(world::Sphere(pos,radius)); i != statics->end() && c < max_lights; ++i) {
		glEnable(GL_LIGHT0 + c);
		(*i)->activate(c);			
		++c;
		++active_statics;
	}				

	for(dynamic_iterator i = dynamics.begin(); i != dynamics.end() && c < max_lights; ++i) {
		if(world::intersect(pos,radius, (*i)->get_pos(), (*i)->get_radius())) {
			glEnable(GL_LIGHT0 + c);
			(*i)->activate(c);			
			++c;
			++active_dynamics;
		}
	}

	for(; c < lights; ++c) {
		glDisable(GL_LIGHT0 + c);
	}

	throw_on_gl_error("light::LightMgrImpl::setup_locals()");
}


void LightMgrImpl::add_global(Light *l)
{ 
	globals.push_back(l); 
}
void LightMgrImpl::add_static(Light *l)
{ 
	n_statics++;
	l->build_triangles(wr); 
	statics->insert(l); 
}

void LightMgrImpl::add_dynamic(Light *l)
{ 
	dynamics.push_back(l); 
}

void LightMgrImpl::remove_global(Light *l) 
{
	globals.remove(l); 
}

void LightMgrImpl::remove_static(Light *l)
{
	n_statics--;
	for(static_iterator i = statics->begin(); i != statics->end(); ++i) {
		if((*i) == l) {
			statics->erase(i);
			return;
		}
	}	
	derr << "FATAL: remove_static(): Light does not exist!";
}

void LightMgrImpl::remove_dynamic(Light *l)
{
	for(dynamic_iterator i = dynamics.begin(); i != dynamics.end(); ++i) {
		if(*i == l) {
			dynamics.erase(i);
			return;
		}
	}
	derr << "FATAL: remove_dynamic(): Light does not exist!";
}	

void LightMgrImpl::render(const world::Frustum &frustum)
{
	using namespace std;

	if(!Settings::current.draw_lights) { 
		return;
	}

	tr->use("smoke");
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	ClientStateKeeper s(GL_TEXTURE_COORD_ARRAY, true);

	/*
	for(dynamic_iterator i = dynamics.begin(); i != dynamics.end(); ++i) {
		if(world::intersect((*i)->get_pos(), (*i)->get_radius(), frustum)) {
			(*i)->build_triangles(wr);
			(*i)->render();
		}
	}
	*/
	
	//for_each(statics->find(frustum), statics->end(), mem_fun_ref(&Light::render));
	const static_iterator end(statics->end());
	for(static_iterator i = statics->find(frustum); i != end; ++i) {
		(*i)->render();
	}

	throw_on_gl_error("light::LightMgrImpl::render()");
}

Color LightMgrImpl::calc_diffuse_light(const Point &p, const Vector &n, const Material &m)
{
	Color c(0,0,0);

	for(global_iterator i = globals.begin(); i != globals.end(); ++i) {
		c += (*i)->calc_diffuse_light(p,n,m);
	}

	for(static_iterator i = statics->begin(); i != statics->end(); ++i) {
		c += (*i)->calc_diffuse_light(p,n,m);
	}

	return clamp(c);
}

int LightMgrImpl::num_static_lights()
{
	return n_statics;
}

} // end lowlevel namespace

LightMgr::LightMgr() : i(new lowlevel::LightMgrImpl()) {}

LightMgr::~LightMgr() { }

LightRef LightMgr::get_ref() { return LightRef(); }

void LightMgr::setup_global()                        { i->setup_global(); }
void LightMgr::setup_locals(const Point& p, float r) { i->setup_locals(p,r); }
void LightMgr::render(const world::Frustum &f)       { i->render(f); }              

void LightMgr::add_global(lowlevel::Light *l)  { i->add_global(l); }
void LightMgr::add_static(lowlevel::Light *l)  { i->add_static(l); }
void LightMgr::add_dynamic(lowlevel::Light *l) { i->add_dynamic(l); }

void LightMgr::remove_global(lowlevel::Light *l)  { i->remove_global(l); }
void LightMgr::remove_static(lowlevel::Light *l)  { i->remove_static(l); }
void LightMgr::remove_dynamic(lowlevel::Light *l) { i->remove_dynamic(l); }

int LightMgr::num_static_lights() { return i->num_static_lights(); }

Color LightMgr::calc_diffuse_light(const Point &p, const Vector &n, const Material &m) {
	return i->calc_diffuse_light(p, n, m); 
}

}
}

/*
 * $Author: pstrand $
 * $Date: 2003/06/11 15:16:42 $
 * $Log: light_mgr.cpp,v $
 * Revision 1.26  2003/06/11 15:16:42  pstrand
 * småfixar..
 *
 * Revision 1.25  2003/06/04 16:29:58  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.24  2002/12/29 17:04:33  pstrand
 * hw/ moved to os/
 *
 * Revision 1.23  2002/05/29 00:35:12  fizzgig
 * cleanup, small fixes, improvement, etc
 *
 * Revision 1.22  2002/05/21 10:04:30  pstrand
 * world.iter.
 *
 * Revision 1.21  2002/04/23 22:50:18  fizzgig
 * mesh-loading cleanup
 *
 * Revision 1.20  2002/04/22 15:16:24  fizzgig
 * remove_xx() only prints diagnostic, does not throw exception.. (lets the game run, but with probable memory leak)
 *
 * Revision 1.19  2002/04/14 19:27:06  pstrand
 * try again... (uniqueptr)
 *
 * Revision 1.18  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.17  2002/02/18 11:51:34  peter
 * initord.
 *
 * Revision 1.16  2002/02/10 13:59:06  macke
 * cleanup
 *
 * Revision 1.15  2002/01/23 04:42:47  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.14  2002/01/22 23:44:01  peter
 * reversed last two revs
 *
 * Revision 1.12  2002/01/16 00:25:26  picon
 * terrängbelysning
 *
 * Revision 1.11  2002/01/11 23:11:09  peter
 * upprensning, mm
 *
 * Revision 1.10  2002/01/11 14:28:07  macke
 * no message
 *
 * Revision 1.9  2001/11/10 21:10:52  peter
 * minnesfixar..
 *
 * Revision 1.8  2001/09/24 02:33:24  macke
 * Meckat lite med fulbuggen i grafikmotorn.. verkar funka att deallokera nu.. ?
 *
 * Revision 1.7  2001/08/20 16:58:38  peter
 * quadtree & world-iter fix
 *
 * Revision 1.6  2001/08/09 18:11:40  macke
 * Fyllefix.. lite här å där..
 *
 * Revision 1.5  2001/08/06 12:16:14  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.4.2.2  2001/08/05 14:01:25  peter
 * objektmeck...
 *
 * Revision 1.4.2.1  2001/07/31 17:34:04  peter
 * testgren...
 *
 * Revision 1.4  2001/07/30 23:43:16  macke
 * Häpp, då var det kört.
 *
 * Revision 1.3  2001/07/23 23:48:10  macke
 * Slimmad grafikhantering samt lite namnbyten
 *
 * Revision 1.2  2001/07/09 13:33:06  peter
 * gcc-3.0 fixar
 *
 * Revision 1.1  2001/07/06 01:47:12  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.21  2001/06/09 01:58:48  macke
 * Grafikmotor reorg
 */
