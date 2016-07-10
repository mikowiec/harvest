
/* $Id: time_types.cpp,v 1.3 2003/01/06 12:42:12 pstrand Exp $ */

#define _POSIX_C_SOURCE 199506L
#include "os/compat.h"

#ifdef WIN32
# include "os/windows.h"
#else

#include <time.h>


#endif

#include "os/time_types.h"





namespace reaper
{
namespace os
{
namespace time
{


TimeSpan TimeSpan::from_us(long us)
{
	return TimeSpan(0, us);
}

TimeSpan TimeSpan::from_ms(long ms) {
	return TimeSpan(ms / low_mod_ms, (ms % low_mod_ms) * 1000);
}

TimeSpan& TimeSpan::operator+=(const TimeSpan& t) {
	high += t.high;
	low += t.low;
	fixup();
	return *this;
}

TimeSpan& TimeSpan::operator-=(const TimeSpan& t) {
	high -= t.high;
	low -= t.low;
	fixup();
	return *this;
}

TimeSpan& TimeSpan::operator*=(double m)
{ 
	high = static_cast<long>(high * m);
	low = static_cast<long>(low * m);
	fixup();
	return *this;
}

TimeSpan operator+(const TimeSpan& t1, const TimeSpan& t2)
{
	return TimeSpan(t1.approx().upper() + t2.approx().upper(),
			t1.approx().lower() + t2.approx().lower());
}

TimeSpan operator-(const TimeSpan& t1, const TimeSpan& t2)
{
	return TimeSpan(t1.approx().upper() - t2.approx().upper(),
			t1.approx().lower() - t2.approx().lower());

}

#ifdef WIN32

void msleep(long t) {
	Sleep(t);
}

#else

void msleep(long t) {
	struct timespec ts;
	ts.tv_sec = t / 1000;
	ts.tv_nsec = (t % 1000) * 1000000;
	nanosleep(&ts, 0);
}

#endif

}
}
}
