
#ifndef REAPER_OBJECT_EVENT_H
#define REAPER_OBJECT_EVENT_H

#include "misc/command.h"
#include "misc/event.h"
#include "object/baseptr.h"

namespace reaper {
namespace object {
namespace event {

class Events
{
public:
	Events();
	~Events();
	typedef misc::Event< misc::Ping > Event;
	Event kill, death, remove;

};

misc::Ping mk_rm_on_death(object::ID id);

}
}
}

#endif

