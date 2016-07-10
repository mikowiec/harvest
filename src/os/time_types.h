
/* $Id: time_types.h,v 1.2 2003/01/06 12:42:12 pstrand Exp $ */

#ifndef REAPER_HW_TIME_TYPES_H
#define REAPER_HW_TIME_TYPES_H

namespace reaper
{
namespace os
{
namespace time
{

const long low_mod_us = 1000000000;
const long low_mod_ms = low_mod_us / 1000;


class TimeApprox {
	long high, low;
public:
	TimeApprox(long h, long l) : high(h), low(l) { }
	operator double() const {
		return static_cast<double>(high) * low_mod_us + low;
	}

	long to_us() const { return high * low_mod_us + low; }
	long to_ms() const { return high * low_mod_ms + low / 1000; }

	double to_s() const { return *this * 1e-6; }

	long upper() const { return high; }
	long lower() const { return low; }
};

/** Time difference.
    Resolution in microseconds, relative to something unspecified.
 */

class TimeSpan {
	long high, low;
	void fixup() 
	{
		if (low > low_mod_us) {
			high += low / low_mod_us;
			low %= low_mod_us;
		} else if (low < 0) {
			high--;
			low += low_mod_us;
		}
	}
public:
	TimeSpan(long h, long l) : high(h), low(l) { fixup(); }

	static TimeSpan from_us(long us);
	static TimeSpan from_ms(long ms);

	TimeSpan() : high(0), low(0) { }

	TimeApprox approx() const { return TimeApprox(high, low); }

	TimeSpan& operator+=(const TimeSpan& t);
	TimeSpan& operator-=(const TimeSpan& t);
	TimeSpan& operator*=(double m);

	bool operator==(const TimeSpan& t) const { return high == t.high && low == t.low; }
	bool operator!=(const TimeSpan& t) const { return !(*this == t); }
	bool operator<(const TimeSpan& t)  const { return high < t.high || (high == t.high && low < t.low); }
	bool operator>(const TimeSpan& t)  const { return high > t.high || (high == t.high && low > t.low); }

};


TimeSpan operator+(const TimeSpan& t1, const TimeSpan& t2);
TimeSpan operator-(const TimeSpan& t1, const TimeSpan& t2);


/// Relative time, in milliseconds (approximately)
typedef long RelTime;


}
}
}
#endif

