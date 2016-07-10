
#include "os/compat.h"
#include "os/debug.h"

#include "object/event.h"

namespace reaper {
namespace object {
namespace event {

namespace { debug::DebugOutput derr("event"); }

struct kill_handler {
	Events& events;
	bool dead;
	kill_handler(Events& e) : events(e), dead(false) { }

	void operator()() {
		if (!dead) {
			dead = true;
			events.death.ping_all();
			events.death.clear();
			events.remove.ping_all();
			events.remove.clear();
		} else {
			// It should never happen anymore, so we print a warning -peter
			derr << &events << "<-   Warning: Multiple kills!\n";
		}
	}
};

Events::Events()
{
	kill.add_listener( misc::mk_cmd(kill_handler(*this)) );
}

Events::~Events()
{
}


}
}
}


