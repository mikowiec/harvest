#ifndef REAPER_PHYS_EDGE_H
#define REAPER_PHYS_EDGE_H

#include <functional>

#include "main/types.h"
#include "main/types.h"
#include "point.h"


namespace reaper
{
namespace phys
{

/// Edge feature of polyhedron
class Edge : public Vector
{
public:
	Edge(Point *tail, Vector v) : tail_pt(tail), Vector(v), unit_norm(norm(v)) {}
  
	Edge(Point *tail, Point *head) : tail_pt(tail), Vector(*head-*tail), unit_norm(norm(*head - *tail)) {} 

	const Point head() const { return *tail_pt + this->to_pt(); }
	const Point &tail() const { return *tail_pt; }

//	Point &head() { return *tail_pt + *this; }
        Point &tail() { return *tail_pt; }

	/// Get unit vector in edge direction
	const Vector &unit_vector() const { return unit_norm; }

	//	Point closest_point_to(const Point &p) const;

	double distance_to(const Point &p) const;
	double distance_to(const Edge &e) const;

//	Edge project_to(const Polygon &p) const;

	//	bool includes(const Point *tail, const Point *head) const;

	Edge &operator=(const Vector &v) { Vector::operator=(v); unit_norm = norm(v); return *this; }

	virtual ~Edge() {} 
protected:
	Edge() : Vector() {}

	Vector unit_norm;

	Point *tail_pt;
};

}
}
#endif
