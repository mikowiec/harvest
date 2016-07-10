#ifndef REAPER_OBJECT_REG_H
#define REAPER_OBJECT_REG_H


#include "phys/engine.h"
#include "phys/accessor.h"
#include "world/world.h"
#include "main/types.h"
#include "object/collide.h"
#include "object/event.h"
#include "misc/command.h"
#include "ai/thinker.h"

#include "os/debug.h"

namespace reaper {
namespace object {

namespace {
	namespace dbg { debug::DebugOutput derr("reg"); }
}

void register_remove(SillyPtr p, misc::Ping rm);

template<class Ptr, class Acc>
void init_reg(Ptr ptr, Acc* acc)
{
	reaper::phys::PhysRef()->insert(acc);
	world::WorldRef()->add_object(ptr);
	register_remove(ptr, event::mk_rm_on_death(ptr->stat().id()));
}


}
}


#endif

