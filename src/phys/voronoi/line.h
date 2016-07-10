#ifndef REAPER_PHYS_LINE_H
#define REAPER_PHYS_LINE_H

#include <utility>

#include "main/types.h"
#include "point.h"

#include "edge.h"

namespace reaper
{
namespace phys
{

class Edge;

/// A line in 3d-space
class Line
{
public:
	Line(const Edge &e);
	Line(const Point &p, const Point &q);

	bool parallel_to(const Line &l);
	double closest_point_param(const Line &l, double &t) const;
	
	Point closest_point_to(const Point *p) const;
	Point closest_point_to(const Line &l, Point *p) const;

	Point param_point(double t) const;
private:
	Vector v;
	Point r0;
};

}
}
#endif

