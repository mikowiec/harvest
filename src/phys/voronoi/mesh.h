#ifndef REAPER_PHYS_MESH_H
#define REAPER_PHYS_MESH_H

#include <utility>
#include <vector>

#include "point.h"
#include "utility.h"

namespace reaper
{
namespace phys
{

class Polyhedron;

/// Low-level data structure for meshes
struct Mesh {
	std::vector<Point> vertices;
	std::vector<std::pair<int, int> > wingeds;
	std::vector<std::vector<int> > faces;
};

Polyhedron* build_polyhedron(const Mesh& m, double alpha);

Polyhedron *cube(double width, double height, double depth);

}
}
#endif


