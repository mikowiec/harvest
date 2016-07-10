#include "os/compat.h"

#include <vector>
#include <limits>

#include "main/types.h"

#include "edge.h"
#include "vertex.h"
#include "line.h"
#include "plane.h"

namespace reaper{

namespace phys{


//  bool Edge::includes(const Point *t, const Point *h) const
//  {
//  	return t == tail_pt && h == head_pt;
//  }

//    Edge::Edge(Point* tail, Point* head) : 
//      tail_pt(tail), Vector(*head-*tail), unit_norm(normalize3(*head - *tail))
//  {
//  }

//  const Vector &Edge::unit_vector() const
//  {
//  	return unit_norm; 
//  }

//  Point Edge::closest_point_to(const Point &p) const
//  {
//  	double t = dot_product3(p-*tail(), unit_vector());
//          return *tail() + unit_vector()*std::min(double(1), std::max(double(0), t));
//  }

//  double Edge::distance_to(const Point &p) const
//  {
//  	return tail()->distance_to(closest_point_to(p));
//  }

//  double Edge::distance_to(const Edge &e) const
//  {
//  	Line a(*this), b(e);
//  	if(!a.parallel_to(b)){
//  		double s, t;
//  		s = a.closest_point_param(b, t);
//  		if(s >= 0 && s <= 1 && t >= 0 && t <= 1)
//  			return a.param_point(s).distance_to(b.param_point(t));
//  	}
//  	double min_dist = std::numeric_limits<double>::max(), dist;
//  	if((dist=head()->distance_to(e.closest_point_to(*head())))<min_dist)
//  		min_dist = dist;
//  	if((dist=tail()->distance_to(e.closest_point_to(*tail())))<min_dist)
//  		min_dist = dist;
//  	if((dist=e.head()->distance_to(closest_point_to(*e.head())))<min_dist)
//  		min_dist = dist;
//  	if((dist=e.tail()->distance_to(closest_point_to(*e.tail())))<min_dist)
//  		min_dist = dist;
//  	return min_dist;
//  }

// Edge Edge::project_to(const Polygon &p) const
//  {
//  	Plane plane(p);
//  	Edge edge = *this;
//  	*edge.head_pt = head()->project_to(plane);
//  	*edge.tail_pt = tail()->project_to(plane);
//  	return edge;
//  }

}
}
