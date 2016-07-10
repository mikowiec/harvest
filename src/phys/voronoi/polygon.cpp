#include "os/compat.h"

#include <functional>
#include <limits>
#include <algorithm>

#include "stlhelper.h"

#include "polygon.h"
#include "utility.h"

#include "plane.h"
#include "edge.h"

namespace reaper
{
namespace phys
{

using namespace std;

using namespace hlp;

//  double Polygon::distance_to(const Point &p) const
//  {
//  	if(p.overlap(*this))
//  		return Plane(*this).distance_to(p);
//  	else
//  		return (*find_closest(begin_edges(), end_edges(), p))->distance_to(p);
//  }

//  bool Polygon::overlap(const Polygon &f) const
//  {
//  	return false;
//  }

//  bool Polygon::is_left_to(const Edge &edge) const
//  {
//  	return edge.tail()==points.front() && edge.head() == points.back() ||
//  	       adjacent_find(points.rbegin(), points.rend(), 
//  			     bind1st3(const_mem_fun2_ref(&Edge::includes), edge)) 
//  		!= points.rend();
//  }

//  bool Polygon::is_right_to(const Edge &edge) const
//  {
//  	return edge.tail()==points.back() && edge.head() == points.front() ||
//  	       adjacent_find(points.begin(), points.end(), 
//  			     bind1st3(const_mem_fun2_ref(&Edge::includes), edge)) 
//  		!= points.end();
//  }


}
}
