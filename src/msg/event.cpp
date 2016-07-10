
#include "os/compat.h"

#include <functional>
#include <string>

#include "misc/sequence.h"
#include "misc/stlhelper.h"
#include "misc/maybe.h"
#include "res/config.h"
#include "res/res.h"
#include "misc/iostream_helper.h"

#include "os/event/event_impl.h"
#include "os/debug.h"
#include "msg/mapping.h"
#include "os/reltime.h"

namespace reaper {
namespace os {
namespace event {

namespace { debug::DebugOutput dlog("event", 5); }

struct event_format_error : public hw_error {
	event_format_error(const std::string& s)
	 : hw_error(std::string("Malformed event-string: ") + s) { }
};





/* Event string-serialize format:
 * event  ::= id ' ' float ' ' int ' ' int
 * id     ::= "0x" hex*4
 */


std::ostream& operator<<(std::ostream& os, const Event& e)
{
	os.precision(3);
	os << "0x" << std::hex << e.id
	   << ' ' << std::dec << e.val.v << ' ' << std::hex << e.time << ' ' << e.recv;
	return os;
}


std::istream& operator>>(std::istream& is, Event& e)
{
	int id, r;
	char junk;
	is >> junk >> junk;
	is >> std::hex >> id >> std::dec >> e.val.v >> std::hex >> e.time >> r;
	e.recv = static_cast<Receiver>(r);
	e.id = static_cast<id::EventID>(id);
	return is;
}



EventSystem::EventSystem(gfx::Gfx& g)
{
	impl = new EventDispatcher(g);
}


EventSystem::~EventSystem()
{
	delete impl;
}

EventProxy EventSystem::get_ref(PlayerID id)
{
	return EventProxy(EventSystem::impl, id);
}

EventProxy* EventSystem::create_ref(PlayerID id)
{
	return new EventProxy(EventSystem::impl, id);
}

EventDispatcher* EventSystem::impl = 0;

EventProxy::EventProxy(EventDispatcher* i, PlayerID id)
 : listener_id(id), state(10)
{
	dlog << "created ep with id " << listener_id << '\n';
	impl = i;
}

EventProxy::~EventProxy() { }

using misc::for_each;
using misc::seq;
using misc::apply_val;



class EventMap : public EventFilterImpl {
	MappingPtr map;
public:
	EventMap(MappingPtr m) : map(m) { }

	bool poll(Event& ev) {
		if (src->poll(ev)) {
			ev = (*map)(ev);
//			dlog << "read event: " << ev << " at " << hw::time::get_rel_time() << '\n';
			return true;
		} else {
			return false;
		}
	}
};

struct IdCmp {
	bool operator()(const Event& e1, const Event& e2) const {
		return e1.id < e2.id;
	}

};

class FixRepeats : public EventFilterImpl {
	MappingPtr map;

	std::queue<Event> pending;

	typedef std::map<Event, os::time::RelTime, IdCmp> Reps;
	Reps repeats;
	os::time::RelTime last_rep;
public:
	FixRepeats(MappingPtr m) : map(m), last_rep(0) { }

	bool poll(Event& ev) {
		os::time::RelTime now = os::time::get_rel_time();

		if (src->poll(ev)) {
			if (map->is_repeat(ev)) {
				if (ev.val > 0.1) {
					repeats[ev] = now;
				} else {
					repeats.erase(ev);
				}
			}
			return true;
		}

		for (Reps::iterator i = repeats.begin(); i != repeats.end(); ++i) {
			if (now > i->second + 20) {
//				dlog << "dorepeat: " << i->first << '\n';
				i->second = now;
				ev = i->first;
				ev.time = now;
				return true;
			}
		}
		return false;
	}
};

void EventSystem::add_filter(EventFilter* f)
{
	impl->add_filter(f);
}

void EventSystem::set_mapping(Mapping* imap)
{
	MappingPtr map(imap);
	impl->add_filter(new FixRepeats(map));
	impl->add_filter(new EventMap(map));
}

void EventSystem::set_mapping(const std::string& id)
{
	Mapping* map = create_game_map(id);
	set_mapping(map);
}

void EventSystem::reconfigure()
{
	impl->rescan();
}

void EventSystem::enable()
{
	impl->enable = true;
	Event e;
	while (impl->filters.back()->poll(e))
		;
}

void EventSystem::disable()
{
	impl->enable = false;
}

class OnlySystem : public EventFilterImpl
{
public:
	bool poll(Event& ev) {
		while (src->poll(ev)) {
			if (ev.recv == event::System)
				return true;
		}
		return false;
	}
};


class PlaybackDevice : public EventFilter
{
	OnlySystem filter;
	EventTimeQueue etq;

	class Reader : public EventFilterImpl {
		res::res_stream rs;
	public:
		Reader(const std::string& name) : rs(res::Playback, name) { }
		bool poll(Event& ev) {
			if (src->poll(ev))
				return true;
			rs >> ev >> misc::crlf;
			return rs.good();
		}
	} read;
public:
	PlaybackDevice(const std::string& name)
	 : read(name)
	{ }

	void set_source(EventSource* src) {
		read.set_source(src);
		filter.set_source(&read);
		etq.set_source(&filter);
	}

	bool poll(Event& ev) {
		return etq.poll(ev);
	}
};

EventFilter* make_playbackdevice(const std::string& name)
{
	return new PlaybackDevice(name);
}

class EventDumper : public EventFilterImpl
{
	res::res_out_stream dump;
public:
	EventDumper()
	 : dump(res::Playback, "latest") // res::gen_name())
	{ }

	bool poll(Event& ev) {
		if (src->poll(ev)) {
			dump << ev << '\n' << std::flush;
			return true;
		}
		return false;
	}
};

EventFilter* make_savedevice()
{
	return new EventDumper();
}


bool EventProxy::get_event(Event& e, bool block)
{
	return impl->get_event(e, listener_id, block);
}

bool EventProxy::key(id::EventID k)
{
	impl->poll(listener_id, ev_table);
	return ev_table[k] > 0.0;
}

bool EventProxy::key(char k)
{
	return key(id::mk(k));
}

bool EventProxy::button(int b)
{
	impl->poll(listener_id, ev_table);
	return ev_table[id::mk(id::Btn0 + b)] > 0.0;
}

float EventProxy::axis(int a)
{
	impl->poll(listener_id, ev_table);
	return ev_table[id::mk(id::Axis0 + a)];
}


bool EventProxy::axis_changed(int a)
{
	float v = axis(a);
	if (v == state[a])
		return false;
	state[a] = v;
	return true;
}


}
}
}


