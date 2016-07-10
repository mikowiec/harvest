
#include "os/compat.h"

#include "os/debug.h"

#include "ai/thinker.h"
#include "misc/sequence.h"

#include <functional>
#include <deque>

namespace reaper {
namespace ai {

namespace {
	reaper::debug::DebugOutput derr("thinker");
}

struct think_c {
	void operator()(Part* t) { t->think(); }
} think_t;

class Thinker::Impl
{
	std::deque<Part*> parts;
public:
	
	void add_part(Part* p)
	{
		parts.push_back(p);
	}
	void rem_part(Part* p)
	{
		parts.erase(misc::find(misc::seq(parts), p));
	}
	~Impl()
	{
		derr << "think size: " << parts.size() << '\n';
	}

	void think()
	{
		misc::for_each(misc::seq(parts), std::mem_fun(&Part::think));
	}
};

Thinker::Thinker()
 : i(new Thinker::Impl())
{ }

Thinker::~Thinker()
{ }

misc::Ping Thinker::add_part(Part* p)
{
	i->add_part(p);
	return misc::mk_cmd(i.get(), &Thinker::Impl::rem_part, p);
}

void Thinker::rem_part(Part* p)
{
	i->rem_part(p);
}

void Thinker::think()
{
	i->think();
}

misc::Ping reg(Part* p)
{
	return ThinkRef()->add_part(p);
}

void unreg(Part* p)
{
	ThinkRef()->rem_part(p);
}


}
}

