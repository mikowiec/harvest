
/* $Id: obj_factory.cpp,v 1.8 2003/03/25 00:29:12 pstrand Exp $ */

#include "os/compat.h"

#include <string>

#include "os/debug.h"
#include "main/types.h"
#include "main/enums.h"
#include "object/base.h"
#include "object/base_data.h"
#include "object/renderer.h"
#include "object/hull.h"
#include "object/factory.h"
#include "object/obj_factory.h"
#include "object/object_impl.h"
#include "res/config.h"
#include "world/world.h"

#include "object/collide.h"

namespace reaper {

namespace object {

namespace factory {

SillyBase* gen_create(const MkInfo& mk)
{
	SillyBase* p = new SillyImpl(mk);
	return p;
}


}


// junk to convince the linker that we need this...
void tree();
void sheep();
void ship();
void turret();
void gvs();
void shot();
void sim_tree();
void lua();

void foo()
{
	tree();
	sheep();
	ship();
	turret();
	gvs();
	shot();
//	sim_tree();
	lua();
}

}
}

