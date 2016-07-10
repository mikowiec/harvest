#ifndef REAPER_GFX_SHADERS_H
#define REAPER_GFX_SHADERS_H

#include <list>
#include "gfx/displaylist.h"

namespace reaper {
namespace gfx {
namespace shaders {

class TexShader
{
	/// One environment per texture unit
	struct TexEnv {
		misc::DisplayList list;
		std::string       texture;
		GLint             prev_env_mode;
	};
	typedef std::list<TexEnv> Cont;

	Cont texenvs;
	TextureRef tr;
public:
	TexShader(const std::string &file);

	void use();	 // set states
	void restore();  // restore states
	void pre_load(); // preload textures into texmgr
};

}
}
}

#endif

/*
 * $Author: fizzgig $
 * $Date: 2002/03/06 21:08:15 $
 * $Log: shaders.h,v $
 * Revision 1.7  2002/03/06 21:08:15  fizzgig
 * Texture-preload now functioning in TexShaders as well..
 *
 * Revision 1.6  2002/01/23 04:42:48  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.5  2002/01/22 23:44:03  peter
 * reversed last two revs
 *
 * Revision 1.3  2001/08/06 12:16:16  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.2.4.1  2001/08/03 13:43:54  peter
 * pragma once obsolete...
 *
 * Revision 1.2  2001/07/06 01:47:14  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.1  2001/06/09 01:58:52  macke
 * Grafikmotor reorg
 *
 */

