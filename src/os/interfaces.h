#ifndef REAPER_HW_INTERFACES_H
#define REAPER_HW_INTERFACES_H

#include "os/time_types.h"

namespace reaper {

namespace debug { class DebugOutput; }

namespace os {

namespace event { struct AxisConfig; }
namespace worker { struct Job; }

namespace ifs {

struct Debug {
	virtual ~Debug() { }
	virtual debug::DebugOutput& derr() = 0;
};

struct Snd : public Debug
{
	virtual std::string config(const std::string& ) = 0;
	virtual void add_job(worker::Job*) = 0;
};

struct Gfx : public Debug {
	virtual std::string config(const std::string& ) = 0;
};

struct Event : public Debug {
	virtual std::string config(const std::string& ) = 0;
	virtual int num_def_axis() const = 0;
	virtual const event::AxisConfig& axis_config(int axis) = 0;
	virtual os::time::RelTime get_time() = 0;
};

}
}
}

#endif
