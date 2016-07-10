/*
 * Lowlevel event handling:
 *  * Scanning and initializing input devices
 *  * Collecthing and dispatching events
 *
 */

#include "os/compat.h"

#include <vector>
#include <queue>
#include <memory>

#include "os/debug.h"

#include "os/event/event_impl.h"
#include "msg/mapping.h"
#include "os/exceptions.h"
#include "os/reltime.h"
#include "os/abstime.h"
#include "os/osinfo.h"
#include "misc/stlhelper.h"
#include "misc/free.h"
#include "misc/parse.h"
#include "res/config.h"

#include "os/gfx/gfx_types.h"
#include "os/gfx/gfx_driver.h"
#include "os/gfx.h"

#include "misc/sequence.h"


namespace reaper {
namespace os {

namespace event {

namespace {
	debug::DebugOutput derr("event_impl");
	debug::DebugOutput dlog("event_impl", 5);
}


AxisConfig::AxisConfig()
: center(32000), min(0), max(64000), dead(1000), sensitivity(1000)
{ }


class MainEvIF_impl : public ifs::Event {
	debug::DebugOutput dr;
	std::vector<AxisConfig> axis_confs;

public:
	MainEvIF_impl() : dr("event"), axis_confs(20) { }

	debug::DebugOutput& derr() { return dr; }

	void read_axis_config(int a, const res::ConfigEnv& conf)
	{
		for (; a< 20; ++a) {
			std::string pfx("axis_" + misc::ltos(a));
			if (!conf.defined(pfx+"dead"))
				return;
			axis_confs[a].dead   = conf[pfx+"dead"];
			axis_confs[a].center = conf[pfx+"center"];
			axis_confs[a].min    = conf[pfx+"min"];
			axis_confs[a].max    = conf[pfx+"max"];
			axis_confs[a].sensitivity = conf[pfx+"sensitivity"];
			
		}
	}

	int num_def_axis() const { return axis_confs.size(); }

	const AxisConfig& axis_config(int axis) {
		return axis_confs[axis];
	}
	os::time::RelTime get_time() {
		return os::time::get_rel_time();
	}
	std::string config(const std::string& lbl) {
		res::ConfigEnv env("hw_input");
		return env[lbl];
	}
};

class InputReader : public EventFilter {
	lowlevel::Gfx_driver_data* gx;
	InputDevices inputs;
	InputDevices::iterator current;
	InputModule input_mod;
	std::auto_ptr<MainEvIF_impl> mev;
public:
	InputReader(lowlevel::Gfx_driver_data* g)
	 : gx(g), mev(new MainEvIF_impl())
	{
		res::ConfigEnv cnf("hw_input");

		std::string pfx("event_");
		std::string sub(cnf["subsystem"]);

		if (sub.empty()) {
			if (os::os_name() == "win32")
	     	   sub = "win32";
	        else
	     	   sub = "x11";
		}

		res::ConfigEnv axis_conf(cnf["joystick"]);
		mev->read_axis_config(2, axis_conf);

		std::auto_ptr<InputDeviceModule> inp(input_mod.create(pfx+sub, mev.get()));

		inp->scan_inputdevices(gx, std::back_inserter(inputs));
		current = inputs.begin();
	}
	~InputReader()
	{
		misc::for_each(misc::seq(inputs), misc::delete_it);
	}

	bool poll(Event& ev) {
		// FIXME, should round-robin or something.. 
		InputDevices::iterator i = inputs.begin();
		for (;i != inputs.end(); ++i) {
			if ((*i)->poll(ev)) {
				ev.time = os::time::get_rel_time();
//				derr << "Reading: " << ev << '\n';
				return true;
			}
		}
		return false;
	}

	void set_source(EventSource*) { }
};

void EventDispatcher::rescan()
{
	if (filters.empty())
		filters.push_back(new InputReader(gx));
	else {
		delete filters[0];
		filters[0] = new InputReader(gx);
	}
	if (filters.size() > 1)
		filters[1]->set_source(filters[0]);
}



EventDispatcher::EventDispatcher(gfx::Gfx& g)
 : gx(g.driver->internal_data()), enable(true)
{
	try {
		rescan();
	} catch (input_error& inp) {
		derr << "input error (while scanning): " << inp.what() << '\n';
	}
}

EventDispatcher::~EventDispatcher()
{
	misc::for_each(misc::seq(filters), misc::delete_it);
	filters.clear();
	misc::for_each(misc::seq(players), misc::delete_it);
	players.clear(); 
}

EventTimeQueue::EventTimeQueue()
 : missed(0), missed_avg(0)
{ }

bool EventTimeQueue::poll(Event& ev) {
	Event e;
	if (src)
		while (src->poll(e))
			q.push(e);

	os::time::RelTime now = os::time::get_rel_time();
	if (q.empty())
		return false;
	if (q.top().time <= now) {
		ev = q.top();
		q.pop();
		if (ev.time < now) {
			++missed;
			Time diff = now - ev.time;
			missed_avg = (missed_avg * (missed-1) + diff) / missed;
			dlog << "Missed event, now:" << std::hex << now << " ev.time:" << ev.time 
			     << " diff:" << std::dec << diff << " nm:"
			     << missed << " avg:" << missed_avg << '\n';
		}
//		if (ev.time < now)
//			dlog << "Missed event!\n";
//			dlog << "pop: " << std::hex << os::time::get_rel_time() << ' ' << ev << '\n';
		return true;
	}
	return false;
}

bool EventTimeQueue::send(const Event& ev)
{
	q.push(ev);
	return true;
}

bool EventTimeQueue::empty() const { return q.empty(); }

/* poll input devices and deliver to appropriate reciever */
void EventDispatcher::poll_inputs()
{
	if (filters.empty()) {
		derr << "no inputreaders installed...\n";
		return;
	}
	EventSource* src = filters.back();

	Event e;
	while (src->poll(e)) {
		if (players.find(e.recv) == players.end())
			players[e.recv] = new EventTimeQueue();
		if (enable || e.recv == System)
			players[e.recv]->send(e);
	}
}


/* Update current-event-tables for a player (temporary) */
void EventDispatcher::poll(PlayerID id, EventTable& table)
{
	Event ev;
	while (get_event(ev, id)) {
		table[ev.id] = ev.val;
	}
}

/* get an event from a playerqueue */
bool EventDispatcher::get_event(Event& e, PlayerID id, bool block)
{
//	derr << "reading events for " << id << '\n';
	poll_inputs();
	if (players.find(id) == players.end()) {
		return false;
	}
	EventSource* my_queue = players[id];
	if (! my_queue->poll(e)) {
		if (block) {
			poll_inputs();
			while (! my_queue->poll(e)) {
				time::msleep(2);
				poll_inputs();
			}
		} else {
			return false;
		}
	}
//	derr << "Deliver: " << e << " at " << os::time::get_rel_time() << '\n';
	return true;
}

void EventDispatcher::add_filter(EventFilter* f)
{
	f->set_source(filters.back());
	filters.push_back(f);
}

}
}
}

