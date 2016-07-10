#include "os/compat.h"

#include "world/world.h"
#include "game/level.h"
#include "world/query_obj.h"
#include "os/debug.h"

#include "misc/sequence.h"
#include "misc/free.h"

#include "gfx/renderer.h"
#include "gfx/interfaces.h"
#include "gfx/exceptions.h"
#include "gfx/camera.h"
#include "gfx/settings.h"
#include "gfx/misc.h"

namespace reaper {
namespace gfx {
namespace lowlevel {

using namespace world;
using namespace reaper::misc;

//--------------------------------------------------------------------
namespace {
reaper::debug::DebugOutput dout("gfx::Renderer",0);

int fog_type(game::FogType f)
{
	switch (f) {
	case game::Linear: return GL_LINEAR;
	case game::Exp:    return GL_EXP;
	case game::Exp_2:  return GL_EXP2;
	case game::NoFog:  return 0;
	default:
		throw gfx_fatal_error("fog_type() - unknown fog type!");
	}
}

} // end anonymous namespace

//--------------------------------------------------------------------
Renderer::Renderer(const game::LevelInfo &li) :
	terrain(li.terrain_mesh, li.terrain_main_texture,
                li.terrain_detail_texture, li.terrain_detail_size),
	sky(li.sky_texture, li.sky_color, li.sky_altitude, li.sky_width),
	shadow(0),
	simul_effects(li.terrain_min_x, li.terrain_min_z, li.terrain_max_x - li.terrain_min_x),
	orphan_effects(li.terrain_min_x, li.terrain_min_z, li.terrain_max_x - li.terrain_min_x),
	stats(this), simul_effects_size(0), orphan_effects_size(0)
{
	for(std::vector<game::LevelInfo::Lake>::const_iterator i = li.lakes.begin(); i != li.lakes.end(); ++i) {
		lakes.push_back(new Lake(i->mesh, i->texture, i->wave_dir, i->amplitude));
	}
}

//--------------------------------------------------------------------
void Renderer::start()
{
	AssertGL agl("IntRndr::start()");

	const game::LevelInfo &li = wr->get_level_info();
	
	int fog = fog_type(li.fog_type);
	if(Settings::current.use_fog && fog) {
		glFogi(GL_FOG_MODE,    fog_type(li.fog_type));
		glFogf(GL_FOG_DENSITY, li.fog_density);
		glFogf(GL_FOG_START,   Settings::current.maximum_view_distance - li.fog_length);
		glFogf(GL_FOG_END,     Settings::current.maximum_view_distance);
		glFogfv(GL_FOG_COLOR,  li.fog_color.get());
	}

	// Misc OpenGL states
	Color clear_col = li.fog_color;
	clear_col.a = 0;
	glClearColor(clear_col);

	glEnable(GL_POINT_SMOOTH);
	glAlphaFunc(GL_GREATER, 0.0);
	glPolygonOffset(-1, -20); //FIXME: these values are ad_hoc
	glEnable(GL_NORMALIZE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
        
	Settings::current.environment_mapping = 
		Settings::current.texture_level == 2 &&
		Settings::current.use_arb_texture_cube_map &&
		Settings::current.use_arb_texture_env_combine ? 
			Settings::current.environment_mapping : 0;
}

//--------------------------------------------------------------------
Renderer::~Renderer()
{
	AssertGL agl("IntRndr::~Renderer()");

	using namespace reaper::misc;

	for_each(seq(orphan_effects), delete_it);
	for_each(seq(lakes),          delete_it);
	for_each(seq(huds), delete_it);
}


//--------------------------------------------------------------------
void Renderer::add_hud(object::ID id, Hud* hud)
{
	huds[id] = hud;
}

//--------------------------------------------------------------------
pm::Pmd* Renderer::get_pmd()
{
	return terrain.get_pmd(); 
}

}
}
}

//--------------------------------------------------------------------
/*
 * $Author: pstrand $
 * $Date: 2003/06/11 15:16:41 $
 * $Log: gfx_init.cpp,v $
 * Revision 1.19  2003/06/11 15:16:41  pstrand
 * småfixar..
 *
 * Revision 1.18  2003/06/04 16:29:58  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.17  2002/12/29 17:04:33  pstrand
 * hw/ moved to os/
 *
 * Revision 1.16  2002/09/20 23:58:08  pstrand
 * no env.map. without texturing...
 *
 * Revision 1.15  2002/05/20 09:59:58  fizzgig
 * dynamic environment mapping!
 *
 * Revision 1.14  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.13  2002/05/10 14:12:50  fizzgig
 * shadow code cleanup
 *
 * Revision 1.12  2002/04/29 20:33:26  fizzgig
 * Added quadtree for particle-systems, plus shot->silly fix
 *
 * Revision 1.11  2002/04/15 10:38:23  fizzgig
 * fog_type() now throws exception for unhandled fog types
 *
 * Revision 1.10  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.9  2002/04/08 21:27:58  pstrand
 * *** empty log message ***
 *
 * Revision 1.8  2002/04/08 20:59:49  pstrand
 * fog-typer lokaliserade...
 *
 * Revision 1.7  2002/03/11 10:50:48  pstrand
 * hw/time.h -> reltime.h&abstime.h, and some more..
 *
 * Revision 1.6  2002/02/26 22:41:17  pstrand
 * mackefix
 *
 * Revision 1.5  2002/02/18 11:52:31  peter
 * level-lake list->vector
 *
 * Revision 1.4  2002/02/13 21:57:18  macke
 * misc effects modifications
 *
 * Revision 1.3  2002/02/08 11:27:49  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.2  2002/02/07 00:02:50  macke
 * STLPort 4.5.1 Win32-fix + shadow & render reorg
 *
 * Revision 1.1  2002/01/28 00:50:39  macke
 * gfx reorg
 *
 * Revision 1.114  2002/01/26 23:27:13  macke
 * Matrox fix, plus some misc stuff..
 *
 * Revision 1.113  2002/01/23 04:42:48  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.112  2002/01/22 23:44:02  peter
 * reversed last two revs
 *
 * Revision 1.110  2002/01/16 00:25:27  picon
 * terrängbelysning
 *
 * Revision 1.109  2002/01/11 14:28:08  macke
 * no message
 *
 * Revision 1.108  2002/01/10 23:09:08  macke
 * massa bök
 *
 * Revision 1.107  2002/01/01 23:28:41  peter
 * 3-stage startup..
 *
 * Revision 1.106  2001/12/24 14:39:38  peter
 * 0.93
 *
 * Revision 1.105  2001/12/17 16:28:32  macke
 * div bök
 *
 * Revision 1.104  2001/12/14 16:31:24  macke
 * meck å pul
 */
