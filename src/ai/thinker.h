
#ifndef REAPER_AI_THINKER_H
#define REAPER_AI_THINKER_H

#include "misc/singleton.h"

#include "ai/part.h"

#include <memory>

namespace reaper {
namespace ai {


class Thinker : public misc::singleton::Base
{
	class Impl;
	std::auto_ptr<Impl> i;
public:
	Thinker();
	~Thinker();
	misc::Ping add_part(Part*);
	void rem_part(Part*);
	void think();
};


typedef misc::singleton::Impl<Thinker> Impl;
typedef misc::singleton::Accessor<Thinker> ThinkRef;

}
}

#endif

