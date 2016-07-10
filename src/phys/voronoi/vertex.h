#ifndef REAPER_PHYS_VERTEX_H
#define REAPER_PHYS_VERTEX_H

#include <vector>

#include "main/types.h"
#include "point.h"

#include "feature.h"

namespace reaper
{
namespace phys
{

class Face;
class Polyhedron;
class Winged;

/// Vertex feature of polyhedron
class Vertex : public Feature, public Point 
{
public:
	typedef std::vector<Winged*>::iterator WingedIterator;
	typedef std::vector<Winged*>::const_iterator ConstWingedIterator;

	typedef std::back_insert_iterator<std::vector<Winged*> > WingedInserter;

	Vertex() : ::reaper::phys::Point::Point(), norm(0, 0, 0) {}

	Vertex(const Point &p) : Point(p), norm(0, 0, 0) {}

	template<class WI> Vertex(const Point &p, WI beg_edgs, WI end_edgs) : Point(p){
		for_each(beg_edgs, end_edgs, back_inserter(wingeds));
	}

	Vertex &operator=(const Point &p) { Point::operator=(p); return *this; }

	Vertex &set_position(const Point& p) { Point::operator=(p); return *this; }

	WingedInserter winged_inserter() { return std::back_inserter(wingeds); }

	WingedIterator begin_wingeds() { return wingeds.begin(); }
	WingedIterator end_wingeds()   { return wingeds.end();   }

	ConstWingedIterator begin_wingeds() const { return wingeds.begin(); }
	ConstWingedIterator end_wingeds()   const { return wingeds.end();   }

	const Vector &normal() const { return norm; }
	Vector &normal() { return norm; }

private:
	std::vector<Winged*> wingeds; ///< Coboundery edges of vertex

	Vector norm;
};

}
}

#endif

