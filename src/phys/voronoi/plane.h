#ifndef REAPER_PHYS_PLANE_H
#define REAPER_PHYS_PLANE_H

#include <cmath>

#include "main/types.h"
#include "main/types_ops.h"

#include "point.h"

namespace reaper
{
namespace phys
{

class Vertex;
class Line;

/// A plane in 3d-space
class Plane 
{
public:
	Plane() {} 

Plane::Plane(const Vector& norm, const Point& p) : 
	a(norm.x), b(norm.y), c(norm.z), d(-p.x*a - p.y*b - p.z*c), p(p)
{
		unit_norm = reaper::norm(norm);
}

	Plane(const Polygon &p);

	const Vector& unit_normal() const { return unit_norm; }

	/// Get distance from plane to a point
	double distance_to(const Point &p) const {
		return (a*p.x + b*p.y + c*p.z + d) / length(Vector(a, b, c));
	}

	operator const double *() const {
		return &a;
	}

	const Point &point() const { return p; }
	Point &point() { return p; }

	Plane flip() const;

//    	Line intersection(const Plane &p) const; 

	double a, b, c, d;
private:
	Vector unit_norm;
	Point p;
};

}
}
#endif
