
/* $Id: abstime.h,v 1.2 2003/01/06 12:42:12 pstrand Exp $ */

#ifndef REAPER_HW_ABSTIME_H
#define REAPER_HW_ABSTIME_H

#include "os/time_types.h"

#include <string>

namespace reaper
{
namespace os
{
namespace time
{


std::ostream& operator<<(std::ostream& os, const TimeSpan& t);
std::istream& operator>>(std::istream& is, TimeSpan& t);



/// sleep t milliseconds (not exact)
void msleep(long t);

/** Get time in microseconds.
    Relative to something unspecified (use diffs...)
 */
TimeSpan get_time();

/** Set reference time. */
void set_time(TimeSpan ref);


/** Current time in string representation. */
std::string strtime(const std::string& format);

}
}
}
#endif

