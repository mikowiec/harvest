
#include "os/compat.h"

#include "object/object_impl.h"
#include "gfx/gfx_types.h"
#include "misc/monitor.h"
#include "misc/stlhelper.h"
#include "misc/parse.h"
#include "object/weapon.h"
#include "object/sound.h"
#include "object/collide.h"
#include "object/event.h"

#include "msg/event.h"
#include "os/debug.h"
#include "main/types_io.h"
#include "res/config.h"

#include "gfx/managers.h"

namespace reaper {
namespace object {

SillyImpl::SillyImpl(const MkInfo& mk)
 : ObjImpl<SillyBase>(mk),
   ri(mk.name, data.get_mtx(), false),
   hull(mk.info["hull_strength"])
{
}

void SillyImpl::collide(CollisionInfo ci)
{
	hull::DamageStatus ds = hull.damage(ci);
	damage(ds, ci, data.get_pos());
	if (ds == hull::Fatality)
		data.events->kill.ping_all();
}

void SillyImpl::initialize()
{
	SillyPtr ptr(this);
	PrimPhysWrap<reaper::phys::PrimAccessor>* a = 
		new PrimPhysWrap<reaper::phys::PrimAccessor> 
			(data.get_id(), stat_acc.get());
	a->add_col(this);
	init_reg(ptr, a);
}

}
}

