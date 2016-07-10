#ifndef REAPER_GFX_GFX_H
#define REAPER_GFX_GFX_H

#include "misc/uniqueptr.h"
#include <memory>
#include <deque>

namespace reaper {
namespace world { class Frustum; }
/// Graphics
namespace gfx {	
namespace pm       { class Pmd; }
namespace lowlevel { class Renderer; } 

class Settings; // see settings.h
class Camera;   // see camera.h
class Initializer;
class Renderer;
typedef reaper::misc::UniquePtr<Renderer> RendererRef;

/// Different types of HUD
enum HUDType {
	None_HUD,
	External_HUD,
	Internal_HUD
};

/// Renderer interface 
class Renderer
{
public:
	// Initialize, but no more.
//	static RendererRef create(const Settings *s = 0);
	static RendererRef get_ref();        
	static Renderer& get_unsafe_ref();
	
	void load();// Load things, but no drawing..	
	void start();// Prepare for rendering.

	void simulate(float dt);
        void render(const Camera &c);

	/// saves the current framebuffer to a png-file in /data/screenshots with current date/time
	void screenshot(); 
	/// return current settings struct (FIXME: allows changing of opengl-ext usage, may cause mayhem!)
	Settings& settings();	

	void purge_textures();
	void purge_meshes();

	pm::Pmd* get_pmd();

        ~Renderer();
	void shutdown();
private:
        Renderer();
	std::auto_ptr<lowlevel::Renderer> i;
	std::deque<Initializer*> inits;
	friend class impl_accessor;
	friend class reaper::misc::UniquePtr<Renderer>;

};

}
}
#endif

/*
 * $Author: fizzgig $
 * $Date: 2002/05/19 17:36:49 $
 * $Log: gfx.h,v $
 * Revision 1.87  2002/05/19 17:36:49  fizzgig
 * glh-lib added (for extensions detect & init)
 * nvparse added
 * extgen added
 * texture blending example added
 *
 * Revision 1.86  2002/05/16 23:58:59  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.85  2002/04/29 20:33:26  fizzgig
 * Added quadtree for particle-systems, plus shot->silly fix
 *
 * Revision 1.84  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.83  2002/01/31 05:35:04  peter
 * *** empty log message ***
 *
 * Revision 1.82  2002/01/27 19:46:02  peter
 * *** empty log message ***
 *
 * Revision 1.81  2002/01/23 04:42:46  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.80  2002/01/22 23:44:00  peter
 * reversed last two revs
 *
 * Revision 1.78  2002/01/01 23:29:00  peter
 * 3-stage startup..
 *
 * Revision 1.77  2001/12/24 14:39:55  peter
 * 0.93
 *
 * Revision 1.76  2001/12/04 23:01:22  picon
 * menu system now working ... barely. :)
 *
 * Revision 1.75  2001/11/10 13:57:02  peter
 * minnesfixar...
 *
 * Revision 1.74  2001/10/08 15:59:18  macke
 * Separerad simulering och rendering för grafikmotorn
 *
 * Revision 1.73  2001/10/04 00:14:11  macke
 * Screenshots åt rätt håll, VC-fix i refptr samt lite städ
 *
 * Revision 1.72  2001/08/06 12:16:12  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.71.2.1  2001/08/03 13:43:52  peter
 * pragma once obsolete...
 *
 * Revision 1.71  2001/07/30 23:43:15  macke
 * Häpp, då var det kört.
 *
 * Revision 1.70  2001/07/27 15:44:51  peter
 * namnbyte..
 *
 * Revision 1.69  2001/07/24 23:52:47  macke
 * Jo, explicit ska det vara (fel på annat ställe)..  rättade till lite array-fel också..
 *
 * Revision 1.68  2001/07/24 17:13:58  peter
 * inge bra med explicit... effects.cpp skapar EffPtr med subklasser till Eff...
 *
 * Revision 1.67  2001/07/23 23:48:10  macke
 * Slimmad grafikhantering samt lite namnbyten
 *
 * Revision 1.66  2001/07/06 01:47:10  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.65  2001/06/09 01:58:47  macke
 * Grafikmotor reorg
 *
 * Revision 1.64  2001/05/14 20:00:52  macke
 * bugfix och rök på missiler..
 *
 * Revision 1.63  2001/05/10 11:40:11  macke
 * häpp
 *
 */

