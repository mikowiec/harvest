#include "main/types.h"
#include "point.h"

namespace reaper
{
namespace phys
{

class Edge;
class Plane;
class Face;
class Polygon;

/// distance between two points
double distance(const Point &a, const Point &b);

/// distance between edge and plane
double distance(const Edge &edge, const Plane &plane);

double distance(const Face &a, const Face &b);

/// parameters for closest points on two 3d lines
void nearest_pts_params(const Point &p, const reaper::Vector &a, double &s,
			const Point &q, const reaper::Vector &b, double &t);


/// nearest point edge to point
Point nearest_pt(const Point &point, const Edge &edge);	

// nearest point polygon to point 
Point nearest_pt(const Point &point, const Polygon &polygon);

// nearest point on edge to plane
Point nearest_pt(const Edge &edge, const Plane &plane);

// nearest point on edge to face
Point nearest_pt(const Edge &edge, const Face &face);

// nearest point on face a to face b
Point nearest_pt(const Face &a, const Face &b);

/// nearest points on two edges
void nearest_pts(const Edge &a, Point &p, const Edge &b, Point &q);


}
}
