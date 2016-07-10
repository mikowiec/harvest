#ifndef REAPER_MISC_UTILITY_H
#define REAPER_MISC_UTILITY_H

#include <iosfwd>
#include <string>
#include <sstream>
#include <cstdlib>

namespace reaper
{
namespace misc
{

template<class strstream>
class clear_strstream
{
	strstream *ss;
public:
	clear_strstream() : ss(new strstream) { ss->setf(std::ios::fixed); ss->precision(0); }
	~clear_strstream() { delete ss; }

	operator strstream& () { return *ss; }
	const std::string str() { return ss->str(); }
	void clear() { delete ss; ss = new strstream; }

	template<class T>
	clear_strstream& operator<<(const T& t) { *ss << t; return *this; }
};

typedef clear_strstream<std::istringstream> clear_istringstream;
typedef clear_strstream<std::ostringstream> clear_ostringstream;
typedef clear_strstream<std::stringstream>  clear_stringstream;

template<class T, class M>
M enum_math(int diff, T &t, const M max)
{
	int current = (int)t;
	current += diff;
	current %= (int)max;
	if(current<0) current += max;
	t = static_cast<M>(current);
	return t;
}

/* 
 * Fake "random" number generator, always generates the same sequence,
 * uniform in the specified interval.
 */

class FakeRand
{
	float min, len;
	long x;
public:
	FakeRand(float min, float max, long seed = 42);
	float operator()();		
};

/*
 * Non-uniform generator (biased towards the center of the interval
 */

class CenterRand
{
	FakeRand rnd;
	float min, len;
public:
	CenterRand(float min, float max, long seed = 42);
	float operator()();
};

/// returns float 0<1
inline float frand()
{
	return rand()/static_cast<float>(RAND_MAX);
}

/// returns float -.5<.5
inline float frand2()
{
	return frand() -.5f;
}

template<class stream, class T>
inline void read_binary(stream &s, T &t)
{
	s.read((char*)&t, sizeof(t));
}

template<class stream, class T>
inline void write_binary(stream &s, const T &t)
{
	s.write((char*)&t, sizeof(t));
}

}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2002/04/06 20:18:18 $
 * $Log: utility.h,v $
 * Revision 1.8  2002/04/06 20:18:18  pstrand
 * *** empty log message ***
 *
 * Revision 1.7  2002/03/24 17:36:42  pstrand
 * "deterministic random" ;)
 *
 * Revision 1.6  2001/08/06 12:16:33  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.5.4.2  2001/08/04 20:37:06  peter
 * flyttad hud, vapenknôk..
 *
 * Revision 1.5.4.1  2001/08/03 13:44:08  peter
 * pragma once obsolete...
 *
 * Revision 1.5  2001/07/21 23:10:01  peter
 * intel fix..
 *
 * Revision 1.4  2001/07/06 01:47:28  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 */
