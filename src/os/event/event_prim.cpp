
/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:13 $
 * $Log: event_prim.cpp,v $
 * Revision 1.2  2003/01/06 12:42:13  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.4  2002/09/29 12:35:52  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.3  2002/04/29 20:29:08  pstrand
 * reorg. and fix
 *
 * Revision 1.2  2002/01/12 02:27:10  peter
 * split time.cpp, plugins oberoende, fixar
 *
 * Revision 1.1  2002/01/11 23:44:43  peter
 * no message
 *
 *
 */

#include "os/compat.h"


#include "msg/event.h"
#include "os/event/event_impl.h"
#include "msg/mapping.h"

namespace reaper
{
namespace os
{
namespace event
{

namespace id {
EventID mk(int i) { return static_cast<EventID>(i); }
}

Mapping::~Mapping()
{ }

EventSource::~EventSource()
{ }

EventSink::~EventSink()
{ }


Event::Event() { }

Event::Event(id::EventID i, bool press)
 : id(i), val(press ? 1.0 : -1.0), time(0), recv(System) { }

Event::Event(int btn, bool press)
 : id(id::mk(id::Btn0 + btn)), val(press ? 1.0 : -1.0), time(0), recv(System) { }

Event::Event(int axis, float v)
 : id(id::mk(id::Axis0 + axis)), val(v), time(0), recv(System)
{
	if (v < -1.0)
		v = -1.0;
	else if (v > 1.0)
		v = 1.0;
}


InputDevice::InputDevice()
 : map(new event::id_map())
{
}

bool InputDevice::set_mapping(MappingPtr m)
{
	map = m;
	return false;
}

bool InputDevice::poll(Event& ev)
{
	if (map == 0)
		return false;
	while (! pending.empty() || poll_device()) {
		ev = pending.front();
		pending.pop();
		return true;
	}
	return false;
}


float norm(float v, const AxisConfig& cf)
{
	if (v < cf.center + cf.dead && v > cf.center - cf.dead)
		return 0;
	if (v < cf.center - cf.dead) {
		return -1 + (v - cf.min) / (cf.center - cf.dead - cf.min);
	} else {
		float lim = cf.center + cf.dead;
		return (v - lim) / (cf.max - lim);
	}
}



bool has_changed(const AxisConfig& ac, int old_pos, int new_pos)
{
	bool less = (new_pos < old_pos - ac.sensitivity);
	bool gt   = (new_pos > old_pos + ac.sensitivity);
	bool oo   = (new_pos > ac.min && new_pos < ac.max);
//	derr << "less: " << less << "  gt: " << gt << "  oo: " << oo << "  " << new_pos << "  " << ac.min << ' ' << ac.max << '\n';
	return (less || gt) && oo;
}




}
	
}
}


