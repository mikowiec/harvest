
/* $Id: reltime.h,v 1.2 2003/01/06 12:42:12 pstrand Exp $ */

#ifndef REAPER_HW_RELTIME_H
#define REAPER_HW_RELTIME_H

#include "os/time_types.h"

#include <string>

namespace reaper
{
namespace os
{
namespace time
{


/// Reset relative time to zero.
void reset_rel_time();

/// Get current relative time.
RelTime get_rel_time();

/// Advance relative time
void rel_time_tick(RelTime ticks);

/** Current time in string representation. */
std::string strtime(const std::string& format);

}
}
}
#endif

