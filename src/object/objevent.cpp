
#include "os/compat.h"

#include "object/event.h"
#include "world/world.h"
#include "phys/engine.h"

namespace reaper {
namespace object {
namespace event {

class OnDeath
{
	object::ID id;
public:
	OnDeath(object::ID i) : id(i) { }
	void operator()() {
		world::WorldRef()->erase(id);
		reaper::phys::PhysRef()->remove(id);
	}
};

misc::Ping mk_rm_on_death(object::ID id) {
	return misc::mk_cmd(OnDeath(id));
}

}

void register_remove(SillyPtr p, misc::Ping rm)
{
	p->events().remove.add_listener(rm);
}

}
}
