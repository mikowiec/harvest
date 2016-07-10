#ifndef REAPER_GFX_PM_PM_TYPES_H
#define REAPER_GFX_PM_PM_TYPES_H

#include <list>

#include "main/types.h"
#include "main/types4.h"
#include "gfx/gfx_types.h"
#include <iomanip>

namespace reaper 
{
namespace gfx
{
namespace pm
{

typedef std::list<int> lint;

/*************************************************************************
* Class Pairdat: Stores Vertex-pair data                                 *
*************************************************************************/

class Pairdat {			//Store data on vertex-pairs.
public:
	lint fl;
	int v[2];
	int ov[2];
	double cost;

	bool operator<(const Pairdat &a) const
	{
		return cost < a.cost;
	}

	bool operator>(const Pairdat &a) const
	{
		return cost > a.cost;
	}

	bool operator==(const Pairdat &a) const
	{
		return (
					((v[0] == a.v[0]) && (v[1] == a.v[1])) ||
					((v[0] == a.v[1]) && (v[1] == a.v[0]))
			   );
	}
};


class wsVertex3
{
public:
	double e[3];

	double len(void){return sqrt(e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);}
	double len_2(void){return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];}
	void normalize(void){double l = len(); e[0] = e[0]/l; e[1] = e[1]/l; e[2] = e[2]/l;}

	
	wsVertex3(void){ e[0] = e[1] = e[2] = 0.0; }

	// covers all vectorbased-types...
	template<class T>
	wsVertex3(const Vec<T> &v){ e[0] = v.x; e[1] = v.y; e[2] = v.z;}

};

class wsMatrix3x3
{
public:
	double e[3][3];

	wsMatrix3x3(void){e[0][0] = e[0][1] = e[0][2] = e[1][0] = e[1][1] = e[1][2] = e[2][0] = e[2][1] = e[2][2] = 0.0;}
};


inline wsVertex3 crossp(const wsVertex3 &a, const wsVertex3 &b)
{
	wsVertex3 r;
	r.e[0] = a.e[1]*b.e[2] - a.e[2]*b.e[1];
	r.e[1] = a.e[2]*b.e[0] - a.e[0]*b.e[2];
	r.e[2] = a.e[0]*b.e[1] - a.e[1]*b.e[0];

	return r;
}

inline wsVertex3 operator+(const wsVertex3 &a, const wsVertex3 &b)
{
	wsVertex3 r;

	r.e[0] = a.e[0] + b.e[0];
	r.e[1] = a.e[1] + b.e[1];
	r.e[2] = a.e[2] + b.e[2];

	return r;
}

inline wsVertex3 operator-(const wsVertex3 &a, const wsVertex3 &b)
{
	wsVertex3 r;

	r.e[0] = a.e[0] - b.e[0];
	r.e[1] = a.e[1] - b.e[1];
	r.e[2] = a.e[2] - b.e[2];

	return r;
}

inline wsVertex3 operator*(double param,wsVertex3 &a)
{
	wsVertex3 r;

	r.e[0] = param* a.e[0];
	r.e[1] = param* a.e[1];
	r.e[2] = param* a.e[2];
	
	return r;
}

inline wsVertex3 operator*(const wsMatrix3x3 &m, const wsVertex3 &v)
{
	wsVertex3 r;

	r.e[0] = m.e[0][0] * v.e[0] + m.e[0][1] * v.e[1] + m.e[0][2] * v.e[2];
	r.e[1] = m.e[1][0] * v.e[0] + m.e[1][1] * v.e[1] + m.e[1][2] * v.e[2];
	r.e[2] = m.e[2][0] * v.e[0] + m.e[2][1] * v.e[1] + m.e[2][2] * v.e[2];

	return r;
}


inline double scalarp(const wsVertex3 &a, const wsVertex3 &b)
{
	return a.e[0]*b.e[0] + a.e[1]*b.e[1] + a.e[2]*b.e[2];
}



typedef std::vector<Pairdat> Vpd;

typedef Vpd::iterator Vpdi;

}
}
}


#endif
