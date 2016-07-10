
#ifndef REAPER_HW_EVENT_PRIM_H
#define REAPER_HW_EVENT_PRIM_H

#include <queue>

#include "msg/event.h"
#include "os/gfx/gfx_driver.h"
#include "os/interfaces.h"
#include "misc/smartptr.h"
#include "os/time_types.h"

namespace reaper {
namespace os {
namespace event {

struct input_error : public hw_error {
	input_error(const std::string& e) : hw_error(e) { }
};

inline
float norm(float a, float b)
{
	float v = ((2.0 * a) / b - 1.0);
	if (v > 1.0)
		return 1.0;
	else if (v < -1.0)
		return 1.0;
	else
		return v;
}

float norm(float, float, float, float, float);


struct AxisConfig
{
	AxisConfig();

	int center, min, max, dead;
	int sensitivity;
};

float norm(float, const AxisConfig&);

bool has_changed(const AxisConfig&, int old_pos, int new_pos);

typedef misc::SmartPtr<Mapping> MappingPtr;

class InputDevice
 : public EventSource
{
protected:
	MappingPtr map;
	std::queue<Event> pending;

	virtual bool poll_device() = 0;
public:
	InputDevice();
	virtual bool set_mapping(MappingPtr);
	bool poll(Event&);
};

class RepInputDevice
 : public InputDevice
{
	typedef std::map<int,Event> Reps;
	Reps repeats;
	ifs::Event* main;
	os::time::RelTime last_rep;
protected:
	RepInputDevice(ifs::Event* m)
	 : main(m), last_rep(0)
	{ }
	virtual bool real_poll_device(std::pair<int,bool>&) = 0;

	bool poll_device()
	{
		std::pair<int,bool> p;
		os::time::RelTime now = main->get_time();
		if (!repeats.empty() && (now > last_rep + 20)) {
			for (Reps::iterator i = repeats.begin(); i != repeats.end(); ++i)
				pending.push((*map)(i->second));
			last_rep = now;
			return true;
		}
		if (! real_poll_device(p)) {
			return false;
		}
		Event e = Event(id::EventID(p.first), p.second);

		if (map->is_repeat(e) && p.second) {
			repeats.insert(std::make_pair(p.first, e));
			return false;
		} else {
			repeats.erase(p.first);
			pending.push((*map)(e));
			return true;
		}

	}
public:

};

typedef std::deque<InputDevice*> InputDevices;
typedef std::back_insert_iterator<InputDevices> device_inserter;

class InputDeviceModule
{
public:
	virtual void scan_inputdevices(lowlevel::Gfx_driver_data*, device_inserter out) = 0;
};



}
}
}

#endif

