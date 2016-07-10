#ifndef REAPER_PHYS_CELL_H
#define REAPER_PHYS_CELL_H

#include "utility.h"
#include "plane.h"

namespace reaper
{
namespace phys
{

class Plane;
class Feature;

/// Half-space cell of Voronoi region
/**
 * A Voronoi region is an infinit partition of space, formed from the
 * intersection of half-spaces.  A cell is such a half-space. 
 */
class Cell 
{
public:
	Cell(const Plane &cplane, Feature* neighbr) :
		cplane(cplane), neighbr(neighbr) {};
	/// Get boundary plane of half-space
	const Plane& constraint_plane() const { return cplane; };

	Plane& constraint_plane() { return cplane; };
	/** Get neighbouring feature whose Voronoi region share
            boundary plane with this cell */
	const Feature *neighbour() const { return neighbr; };
private:
	Plane cplane;
	Feature *neighbr; ///< Next feature if this applicability test fails

};

}
}
#endif
