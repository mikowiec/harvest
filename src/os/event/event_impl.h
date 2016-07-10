#ifndef REAPER_HW_EVENT_IMPL_H
#define REAPER_HW_EVENT_IMPL_H

#include <vector>
#include <queue>
#include <deque>
#include <map>

#include "msg/event.h"
#include "os/gfx/gfx_driver.h"
#include "os/interfaces.h"
#include "misc/plugin.h"
#include "misc/smartptr.h"
#include "os/event/event_prim.h"

#include <stdio.h>

namespace reaper {
namespace os {
namespace gfx { class Gfx; }

/// Event handling.(internal).
namespace event {




typedef misc::Plugin<InputDeviceModule, ifs::Event> InputModule;

class MainEvIF_impl;

template<class P>
class SourceImpl : public P
{
protected:
	EventSource* src;
public:
	SourceImpl() : src(0) { }
	void set_source(EventSource* s) { src = s; }
};

typedef SourceImpl<EventFilter> EventFilterImpl;

class EventTimeQueue : public SourceImpl<EventQueue>
{
	std::priority_queue<Event, std::vector<Event>, std::greater<Event> > q;
	int missed;
	Time missed_avg;
public:
	EventTimeQueue();
	bool poll(Event& ev);
	bool send(const Event& ev);
	bool empty() const;
};


typedef std::deque<EventSource*> InputSources;
typedef std::deque<EventFilter*> Filters;
typedef std::map<PlayerID, EventQueue*> PlayerQueues;

///
class EventDispatcher
{
	friend class EventProxy;
	friend class EventSystem;
	lowlevel::Gfx_driver_data* gx;

	Filters filters;

	/** Eventconsuming players.  */
	PlayerQueues players;

	bool enable;

	void deliver(EventSource*);
public:
	void rescan();

	void poll(PlayerID id, EventTable& table);
	bool get_event(Event& e, PlayerID id, bool block = false);

	EventDispatcher(gfx::Gfx& gx);
	~EventDispatcher();
	void poll_inputs();

	void add_filter(EventFilter*);
};

}
}
}

#endif

