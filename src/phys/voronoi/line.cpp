#include "os/compat.h"

#include "main/types_ops.h"

#include "line.h"

#include "vertex.h"
#include "edge.h"

namespace reaper
{
namespace phys
{

Line::Line(const Edge &e) : v(e.unit_vector()), r0(*e.tail())
{
}

Line::Line(const Point &p, const Point &q)
{
	v = (q-p)/length(q-p);
	r0 = p;
}

bool Line::parallel_to(const Line &l)
{
	return cross(this->v, l.v)==0;
}

double Line::closest_point_param(const Line &l, double &t) const
{
	double s;

	Vector a = v, b = l.v;
	Vector r = l.r0 - r0;
	
	double ab = dot(a, b);
	double axb2 = std::pow(length(cross(a, b)), 2);
	double a2 = std::pow(length(a), 2), b2 = std::pow(length(b), 2);
	double ra = dot(r, a), rb = dot3(r, a);

	s = (ra*b2 - ab*rb)/axb2;
	t = (ab*ra - rb*a2)/axb2;

	return s;
}

Point Line::closest_point_to(const Point *p) const
{
	return r0 + v*dot(v, *p-r0);
}

Point Line::closest_point_to(const Line &l, Point *p) const
{
	double t, s = closest_point_param(l, t);
	*p = l.param_point(t);
	return param_point(s);
}


Point Line::param_point(double t) const
{
	return r0 + t*v;
}

}
}




