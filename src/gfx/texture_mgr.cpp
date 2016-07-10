/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:36 $
 * $Log: texture_mgr.cpp,v $
 * Revision 1.16  2002/12/29 17:04:36  pstrand
 * hw/ moved to os/
 *
 * Revision 1.15  2002/09/23 12:08:43  fizzgig
 * Moved all anonymous namespaces into the reaper-namespace
 *
 * Revision 1.14  2002/09/20 23:46:59  pstrand
 * use_texturing->texture_level, minor event fix.
 *
 * Revision 1.13  2002/05/16 23:59:00  fizzgig
 * Preliminary dynamic cubemapping
 *
 * Revision 1.12  2002/04/14 19:27:06  pstrand
 * try again... (uniqueptr)
 *
 * Revision 1.11  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.10  2002/04/08 20:07:08  pstrand
 * valarray -> vector<float>  (just for compat with egcs, with is the only place valarray is used...)
 *
 * Revision 1.9  2002/02/28 12:40:09  fizzgig
 * shadow optimization + better texture statistics
 *
 * Revision 1.8  2002/02/26 22:41:17  pstrand
 * mackefix
 *
 * Revision 1.7  2002/02/08 11:27:54  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.6  2002/01/10 23:09:09  macke
 * massa bök
 *
 * Revision 1.5  2001/12/14 16:31:25  macke
 * meck å pul
 *
 * Revision 1.4  2001/11/10 21:10:52  peter
 * minnesfixar..
 *
 * Revision 1.3  2001/08/06 12:16:16  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.2.4.1  2001/08/04 23:19:31  peter
 * *** empty log message ***
 *
 * Revision 1.2  2001/07/06 01:47:15  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.1  2001/06/09 01:58:54  macke
 * Grafikmotor reorg
 *
 * Revision 1.20  2001/06/07 05:14:21  macke
 * Reaper v0.9
 *
 * Revision 1.19  2001/05/10 11:40:18  macke
 * häpp
 *
 */

#include "os/compat.h"

#include "main/types.h"

#include <vector>

#include "gfx/managers.h"
#include "gfx/texture.h"
#include "gfx/exceptions.h"
#include "gfx/settings.h"
#include "os/debug.h"
#include "os/gl.h"
#include "misc/map.h"
#include "misc/creator.h"

#include "misc/sequence.h"
#include "misc/free.h"

namespace reaper {
namespace {
        debug::DebugOutput dout("gfx::texture::Manager",0);
}
namespace misc { 
    UniquePtr<gfx::TextureMgr>::I UniquePtr<gfx::TextureMgr>::inst;
}
namespace gfx {
namespace lowlevel {

using namespace texture;
using misc::CreateBase;
using misc::Creator;

/// Manages loading/deleting of file textures
class TextureMgrImpl
{
	typedef misc::Map<Unique,Texture2D> TextureCont;
	typedef std::map<Unique, CreateBase<Texture2D>*> TextureCreators;

	TextureCont     manager;
	TextureCreators spec_textures;
public:
	TextureMgrImpl();
	~TextureMgrImpl();

	void use(Unique id, int level);
	Texture2D *load(Unique id, int level);

	int count() const;
	void purge();

	int total_size() const;
	int resident_size() const;		
	int resident_num() const;
};

TextureMgrImpl::TextureMgrImpl() : 
	manager("gfx::texture")
{
	spec_textures["smoke"] = new misc::Creator<Texture2D, Smoke>();
}

TextureMgrImpl::~TextureMgrImpl()
{
	misc::for_each(misc::seq(spec_textures), misc::delete_it);
	throw_on_gl_error("texture::TextureMgrImpl::~TextureMgrImpl()");
}

inline void TextureMgrImpl::use(Unique id, int level)
{
	if(id.get_text() == "" || Settings::current.texture_level < level) {
		glDisable(GL_TEXTURE_2D);
	} else {
		glEnable(GL_TEXTURE_2D);
		load(id, level)->use();
	}
}

inline Texture2D* TextureMgrImpl::load(Unique id, int level)
{
	//FIXME: Potential bug, but we are ok as long as we only use the
	//       texture ptr if texturing is enabled. Feels unsafe.
	if(Settings::current.texture_level < level)
		return 0;

	Texture2D *t = manager.get_if(id);

	if(t == 0) {
		if(spec_textures.find(id) != spec_textures.end()) {
			t = &manager.insert(spec_textures[id]->create(), id);
		} else {
			t = &manager.insert(new Texture(id), id);
		}
	}
	
	return t;
}

inline int TextureMgrImpl::count() const
{
	return manager.size();
}

inline void TextureMgrImpl::purge()
{
	manager.purge();
}


} // end namespace lowlevel

TextureMgr::TextureMgr() : i(new lowlevel::TextureMgrImpl) {}
TextureMgr::~TextureMgr() { }

/*
TextureRef TextureMgr::create()  {
	TextureRef r;
	if (!r.valid())
		TextureRef::create(new TextureMgr());
	return r;
}
*/

TextureRef TextureMgr::get_ref() { return TextureRef(); }

void TextureMgr::use(Unique id, int level)  { i->use(id, level); }
void TextureMgr::load(Unique id, int level) { i->load(id, level); }

int  TextureMgr::count() const         { return i->count(); } 
void TextureMgr::purge()               { i->purge(); }

}
}
