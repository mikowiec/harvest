
#include "os/compat.h"

#include "main/types.h"
#include "object/base.h"
#include "object/factory.h"
#include "object/projectile.h"

namespace reaper {
namespace object {



factory::ObjBase* laser(MkInfo mk)
{
	return new Laser(mk.mtx, mk.cid, -1);
}

factory::ObjBase* light_missile(MkInfo mk)
{
	return new Missile("light_missile", mk.mtx, mk.cid, -1);
}

factory::ObjBase* heavy_missile(MkInfo mk)
{
	return new Missile("heavy_missile", mk.mtx, mk.cid, -1);
}


struct Foo {
	Foo() {
		factory::inst().register_object("laser", laser);
		factory::inst().register_object("light_missile", light_missile);
		factory::inst().register_object("heavy_missile", heavy_missile);
	}
} bar;

void shot() { }

}
}

