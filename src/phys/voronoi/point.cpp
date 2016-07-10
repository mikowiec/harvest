#include "os/compat.h"

#include <vector>
#include <functional>
#include <algorithm>

#include "point.h"
#include "stlhelper.h"


#include "main/types.h"
#include "plane.h"
#include "polygon.h"
#include "edge.h"

namespace reaper
{
namespace phys
{

using namespace std;

//  double Point::distance_to(const Point &p) const
//  {
//  	return length3(p - *this);
//  }

//  Point Point::project_to(const Plane &plane) const
//  {
//  	Point p(0, 0, -plane.d/plane.c);
//  	const Vector &n = plane.unit_normal();
//  	return (*this - p) - dot_product3(*this-p, n)*n;
//  }

//  class ConstraintPlaneMaker : 
//  public std::unary_function<const Edge*, Plane>
//  {
//  	const Polygon polygon;
//  public:
//  	ConstraintPlaneMaker(const Polygon &p) : polygon(p) {}
	
//  	Plane operator()(const Edge *edge) const {
//  		Vector normal;
//  		if(polygon.is_left_to(*edge))
//  			normal = cross_product(polygon.normal(), edge->unit_vector());
//  		else
//  			normal = cross_product(edge->unit_vector(), polygon.normal());
//  		return Plane(normal, *edge->tail());
//  	}
//  };

//  bool Point::overlap(const Polygon &polygon) const
//  {
//  	vector<Plane> constraint_planes;

//  	transform(polygon.begin_edges(), polygon.end_edges(), 
//  		  back_inserter(constraint_planes), 
//  		  ConstraintPlaneMaker(polygon));
	
//    	return find_if(constraint_planes.begin(), constraint_planes.end(),
//  		       hlp::compose1(bind2nd(less<double>(), 0), 
//  				     bind2nd(mem_fun_ref(&Plane::distance_to), *this)))
//  		!= constraint_planes.end();
//  }

}
}






