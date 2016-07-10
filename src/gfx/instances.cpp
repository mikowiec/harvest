/*
 * $Author: pstrand $
 * $Date: 2003/06/04 16:29:58 $
 * $Log: instances.cpp,v $
 * Revision 1.9  2003/06/04 16:29:58  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.8  2002/12/29 17:04:33  pstrand
 * hw/ moved to os/
 *
 * Revision 1.7  2002/02/08 11:27:51  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.6  2001/12/11 23:16:46  macke
 * engine tails now disappear properly opon death...
 *
 * Revision 1.5  2001/10/01 17:36:31  macke
 * Hmm.. grafikmotorn äger objekt som håller en ref-ptr till grafikmotorn.. fel!
 *
 * Revision 1.4  2001/09/24 02:33:24  macke
 * Meckat lite med fulbuggen i grafikmotorn.. verkar funka att deallokera nu.. ?
 *
 */

#include "os/compat.h"
#include "main/types.h"
#include "gfx/abstracts.h"
#include "gfx/instances.h"
#include "gfx/renderer.h"
#include "os/debug.h"

namespace reaper {
namespace {
debug::DebugOutput dout("gfx::instance");
}

namespace gfx {

class hud_reg_bah : public impl_accessor
{
public:
	hud_reg_bah(object::ID id, Hud* h)
	{
		ref().add_hud(id, h);
	}
};

void hud_reg(object::ID id, Hud* h)
{
	hud_reg_bah(id, h);
};



void EffectPtr::draw() const 
{	
	ref().draw(get()); 
}

void SimEffectPtr::insert() const   
{
	ref().insert(get()); 
}
void SimEffectPtr::insert_release()
{ 
	ref().insert_orphan(release()); owned = false; 
}
void SimEffectPtr::transfer()       
{
	ref().transfer(release()); owned = false; 
}
void SimEffectPtr::remove() const   
{
	ref().remove(get()); owned = false; 
}

SimEffectPtr::~SimEffectPtr() {
	if(owned && get() != 0) {
		transfer();
	}
}


}
}
