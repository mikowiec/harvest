#include "os/compat.h"

#include "main/types.h"

#include "plane.h"

#include "polygon.h"
#include "vertex.h"
#include "edge.h"
#include "line.h"

namespace reaper
{
namespace phys
{

Plane Plane::flip() const
{
	return Plane(-unit_norm, Point(0, 0, -d/c));
}

Plane::Plane(const Polygon &p)
{
	Plane(p.normal(), (*p.begin_edges())->tail());
	this->p = (*p.begin_edges())->tail();
	
}

//  Plane::Plane(const Vector& norm, const Point& p) : 
//  	a(norm.x), b(norm.y), c(norm.z), d(-p.x*a - p.y*b - p.z*c), p(p)
//  {
//  	unit_norm = normalize3(norm);
//  }

//  double Plane::distance_to(const Point &p) const
//  {
//  	return std::fabs(a*p.x + b*p.y + c*p.z + d);
//  }

//  Line Plane::intersection(const Plane &p) const
//  {
//  	double e = p.a, f = p.b, g = p.c, h = p.d;
//  	Vector v((g*b - c*f)/(a*f - e*b), (a*g - c*e)/(b*e - a*f), 1);
//  	Point r0((h*b - d*f)/(a*f - e*b), (a*h - d*e)/(b*e - a*f), 1);
//  	return Line(r0, r0 + v);
	
//  }

}
}
