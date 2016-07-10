#ifndef REAPER_PHYS_POINT_H
#define REAPER_PHYS_POINT_H

#include "main/types_ops.h"

namespace reaper
{
namespace phys
{

class Plane;
class Polygon;

class Point : public ::reaper::Point
{
public:
	Point() : ::reaper::Point() {}
	Point(double x, double y, double z, double w = 1) : ::reaper::Point(x, y, z) {}
	Point(const Vector &p) : ::reaper::Point(p.to_pt()) {}
	Point(const Point& p) : ::reaper::Point(p) {}

	//	double distance_to(const Point &p) const;
	//	Point project_to(const Plane &p) const;
	//	bool overlap(const Polygon &p) const;

	virtual ~Point() {}
};

}
}

#endif
