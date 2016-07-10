#include "os/compat.h"

#include "os/gl.h"
#include "os/gl/gl_info.h"
#include "os/debug.h"

#include "gfx/gfx.h"
#include "gfx/managers.h"
#include "gfx/renderer.h"
#include "gfx/settings.h"
#include "gfx/exceptions.h"
#include "gfx/initializers.h"

#include "misc/stlhelper.h"
#include "misc/sequence.h"

#include "world/world.h"
#include "game/level.h"

#include <map>
#include <string>

//-----------------------------------------------------------------
namespace reaper {
namespace misc {
	UniquePtr<gfx::Renderer>::I UniquePtr<gfx::Renderer>::inst;
}

namespace gfx {	
namespace {
debug::DebugOutput dout("gfx::Renderer", 0);
} // end anonymous namespace


//-----------------------------------------------------------------
RendererRef Renderer::get_ref()
{
	return RendererRef();
}

//-----------------------------------------------------------------
Renderer& Renderer::get_unsafe_ref()
{
	RendererRef r;
	if(!r.valid()) {
		throw gfx_fatal_error("Renderer::get_unsafe_ref(). Renderer not initialized!\n");
	}
	return *r;
}

//-----------------------------------------------------------------
Renderer::Renderer() : i(0) 
{
	using namespace reaper::misc;

	init_gl_settings(Settings::current);

	// Register and handle initializers
	reaper::misc::push_back(inits) 
		<< reaper::gfx::initializers::shot
		<< reaper::gfx::initializers::sphere
		<< reaper::gfx::initializers::font_init;
	

	Seq<std::deque<Initializer*>::iterator> si(seq(inits));
	while (si)
		(*si++)->init();
}
//-----------------------------------------------------------------
void Renderer::shutdown() 
{
	delete i.release();

	for(std::deque<Initializer*>::iterator i = inits.begin(); i != inits.end(); ++i) 
		(*i)->exit();

	LightRef::destroy();
	MeshRef::destroy();
	TextureRef::destroy();
}
Renderer::~Renderer() 
{
}

//-----------------------------------------------------------------
void Renderer::load() 
{ 
	const game::LevelInfo &li = world::WorldRef()->get_level_info();

	TextureRef::create();
	MeshRef::create();
	LightRef::create();
	i = std::auto_ptr<lowlevel::Renderer>(new lowlevel::Renderer(li));
}
void Renderer::start()                  { i->start();              }
void Renderer::simulate(float dt)       { i->simulate(dt);         }
void Renderer::render(const Camera &c)  { i->render(c);            }
void Renderer::purge_textures()         { TextureMgr::get_ref()->purge(); }
void Renderer::purge_meshes()           { MeshMgr::get_ref()->purge();    }

pm::Pmd*  Renderer::get_pmd()  { return i->get_pmd();      }
Settings& Renderer::settings() { return Settings::current; }

//-----------------------------------------------------------------

}
}

/*
 * $Author: pstrand $
 * $Date: 2003/06/11 15:16:39 $
 * $Log: gfx.cpp,v $
 * Revision 1.91  2003/06/11 15:16:39  pstrand
 * småfixar..
 *
 * Revision 1.90  2003/06/04 16:29:57  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.89  2003/01/11 23:58:43  fizzgig
 * improved opengl implementation reporting
 *
 * Revision 1.88  2002/12/29 17:04:32  pstrand
 * hw/ moved to os/
 *
 * Revision 1.87  2002/09/24 19:55:35  pstrand
 * networking fixes
 *
 * Revision 1.86  2002/06/06 07:59:38  pstrand
 * gl init, settings, fix
 *
 * Revision 1.85  2002/05/19 17:36:49  fizzgig
 * glh-lib added (for extensions detect & init)
 * nvparse added
 * extgen added
 * texture blending example added
 *
 * Revision 1.84  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.83  2002/04/29 20:33:26  fizzgig
 * Added quadtree for particle-systems, plus shot->silly fix
 *
 * Revision 1.82  2002/04/14 19:27:06  pstrand
 * try again... (uniqueptr)
 *
 * Revision 1.81  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.80  2002/03/10 14:47:49  pstrand
 * use correct settings when initializing..
 *
 * Revision 1.79  2002/01/31 05:35:04  peter
 * *** empty log message ***
 *
 * Revision 1.78  2002/01/28 00:50:38  macke
 * gfx reorg
 *
 * Revision 1.77  2002/01/23 04:42:46  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.76  2002/01/22 23:44:00  peter
 * reversed last two revs
 *
 * Revision 1.74  2002/01/17 04:56:38  peter
 * pos debugprio = less important.. (changed)
 *
 * Revision 1.73  2002/01/11 23:11:08  peter
 * upprensning, mm
 *
 * Revision 1.72  2002/01/11 14:21:43  peter
 * disable debug
 *
 * Revision 1.71  2002/01/10 23:09:06  macke
 * massa bök
 *
 * Revision 1.70  2002/01/01 23:29:00  peter
 * 3-stage startup..
 */
