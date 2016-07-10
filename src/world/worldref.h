#ifndef REAPER_WORLD_WORLDREF_H
#define REAPER_WORLD_WORLDREF_H

#include "misc/singleton.h"

namespace reaper {

namespace world {
	class World;
	typedef misc::singleton::Impl<World> Impl;
	typedef misc::singleton::Accessor<World> WorldRef;

}
}


#endif
