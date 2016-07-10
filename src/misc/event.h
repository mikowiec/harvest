
#ifndef REAPER_MISC_EVENT_H
#define REAPER_MISC_EVENT_H

#include <deque>

#include "os/debug.h"

namespace reaper {
namespace misc {

struct LocalPing
{
	template<class T>
	void operator()(T t)
	{
		t();
	}
};

namespace {
	namespace ev {
		debug::DebugOutput derr("event");
	}
}

struct Wrap
{
	Ping p;
	Wrap(Ping pi) : p(pi) { }
};
struct Wrap2
{
	Wrap* w;
};

struct CallIfNZ
{
	SmartPtr<Wrap2> sp;
	CallIfNZ(SmartPtr<Wrap2> s) : sp(s) { }
	CallIfNZ(const CallIfNZ& c) : sp(c.sp) { }

	void operator()() const {
		if (sp.valid() && sp->w)
			sp->w->p();
	}
};

template<class Cmd>
class Event
{
	typedef std::deque<Cmd> CmdQ;
	CmdQ listeners;
	std::deque< SmartPtr<Wrap2> > rm_notify;

	void remove_listener(int idx)
	{
		listeners[idx] = Ping();
	}
public:
	Event() { }
	~Event() {
		clear();
	}
	void ping_all()
	{
		for_each(seq(listeners), LocalPing());
	}

	Ping add_listener(Cmd cmd)
	{
		int idx = listeners.size();
		listeners.push_back(cmd);
		Wrap2* w2 = new Wrap2;
		w2->w = new Wrap(mk_cmd( this, &Event::remove_listener, idx) );
		SmartPtr<Wrap2> sp(w2);
		rm_notify.push_back(sp);
		return mk_cmd(CallIfNZ(sp));
	}
	void clear()
	{
		for (int i = 0; i < rm_notify.size(); ++i) {
			delete rm_notify[i]->w;
			rm_notify[i]->w = 0;
		}
		rm_notify.clear();
		listeners.clear();
	}
};



}
}

#endif


