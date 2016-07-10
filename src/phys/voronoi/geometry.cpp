#include "os/compat.h"

#include "geometry.h"

#include <iostream>
#include <limits>
#include <typeinfo>
#include <cmath>

#include "distance.h"

#include "polyhedron.h"
#include "face.h"
#include "vertex.h"
#include "edge.h"
#include "plane.h"
#include "cell.h"
#include "winged.h"

namespace reaper
{
namespace phys
{

using namespace std;

double distance(const Edge &edge, const Plane &plane)
{
	cout << "Calculating distance between edge and plane." << endl;
	return fabs(plane.distance_to(nearest_pt(edge, plane)));
}

double distance(const Face &a, const Face &b)
{
	cout << "Calculating distance between two faces." << endl;
	return Plane(b).distance_to(nearest_pt(a, b));
}

Point nearest_pt(const Face &a, const Face &b)
{
	cout << "Finding nearest point on face to face." << endl;
	if(fabs(dot_product3(a.normal(), b.normal())) == 1)
		return *(*a.begin_vertices()); 
	else {
		Point nearest;
		double min = numeric_limits<double>::max(); 
		for(Face::ConstVertexIterator i = a.begin_vertices(); i != a.end_vertices(); ++i){
			Point p = nearest_pt(*(*i), b);
			double test = distance(p, *(*i));
			if(test<min){
				min = test;
				nearest = *(*i);
			}
		}
		for(Face::ConstWingedIterator i = a.begin_wingeds(); i != a.end_wingeds(); ++i){
			Point p = nearest_pt(*(*i), b);
			double test = distance(p, *(*i));
			if(test<min){
				min = test;
				nearest = p;
			}
		}
		return nearest;
	}
}



double distance(const Point &a, const Point &b){
	cout << "Calculating distance between points." << endl;
	return fabs(length3(b-a));
}

void nearest_pts_params(const Point &p, const reaper::Vector &a, double &s,
			const Point &q, const reaper::Vector &b, double &t)
		       
{
	cout << "Calculating nearest points-parameters." << endl;
	const reaper::Vector r = q - p;
	double ra   =   dot_product3(r, a), rb =   dot_product3(r, b), ab = dot_product3(a, b),
	       a2   = pow(length3(a), 2.0), b2 = pow(length3(b), 2.0), 
	       axb2 = pow(length3(cross_product(a, b)), 2);
	
	t = (ra * ab - rb * a2) / axb2;
	s = (ra * b2 - rb * ab) / axb2;
}

Point nearest_pt(const Point &point, const Edge &edge)
{
	cout << "Finding nearest point on edge to a point." << endl;
	float proj = dot_product3(point - edge.tail(), edge.unit_vector());
	double param = min(length3(edge), max(proj, static_cast<float>(0.0)));
	return edge.tail() + param*edge.unit_vector();
}

void nearest_pts(const Edge &a, Point &p, const Edge &b, Point &q)
{
	cout << "Finding nearest points on two edges." << endl;
	if(dot_product3(a, b)==0.0){
		p = a.tail() + 0.5*a;
		q = b.tail() + 0.5*b;
	}
	else{
		double s, t;
		nearest_pts_params(a.tail(), a.unit_vector(), s, b.tail(), b.unit_vector(), t);
		if( s > 0.0 && s < length3(a) && t > 0.0 && t < length3(b)){
			p = a.tail() + a.unit_vector()*s;
			q = b.tail() + b.unit_vector()*t;
		}
		else{
			p = nearest_pt(b.tail(), a);
			q = b.tail();
			double min = distance(p, q);
			Point test = nearest_pt(b.head(), a);
			double t = distance(test, b.head());
			if(t<min){
				min = t;
				p = test;
				q = b.head();
			}
			test = nearest_pt(a.tail(), b);
			t = distance(a.tail(), test);
			if(t<min){
				min = t;
				q = test;
				p = a.tail();
			}
			test = nearest_pt(a.head(), b);
			t = distance(a.head(), test);
			if(t<min){
				min = t;
				q = test;
				p = a.head();
			}
		}
	}
}

Point nearest_pt(const Point &point, const Polygon &polygon)
{
	cout << "Finding nearest point on face to a point." << endl;
	typedef Polygon::ConstPointIterator PI; 
	bool overlap = true;
	double min = numeric_limits<double>::max();
	
	Point nearest;

	PI prev = (polygon.end_points() -1);
	for(PI curr = polygon.begin_points(); curr != polygon.end_points(); ++curr){
		const Edge edge(*prev, *curr);
		const reaper::Vector normal = cross(edge, polygon.normal());
		const Point near = nearest_pt(point, edge);
		double test = distance(near, point);
		if(test<min){
			min = test;
			nearest = near;
		}
		if(overlap && Plane(normal, *(*curr)).distance_to(point)<0)
			overlap = false;
		prev = curr;
	}	
	if(overlap){
		Point a = *(*polygon.begin_points());
		reaper::Vector r = point - a;
		double proj = dot(norm(polygon.normal()), r);
		return a + r - proj*norm(polygon.normal());
	}
	else
		return nearest;

}

Point nearest_pt(const Edge &edge, const Plane &plane)
{
	cout << "Finding nearest point on edge to a plane." << endl;
	double norm = dot(edge, plane.unit_normal());
	double dist = plane.distance_to(edge.head());
	if(norm!=0){
		double ndist = dist/norm;
		if(ndist <= length3(edge)){
			if(ndist >= 0)
				return edge.head() - ndist*edge.unit_vector();
			else
				return edge.head();
		}
		else
			return edge.tail();
	}
	else
		return edge.head();
}

Point nearest_pt(const Edge &edge, const Face &face)
{
	cout << "Finding nearest point on edge to face." << endl;
	Point p = nearest_pt(edge, Plane(face));
	if(!point_cell_checkp(p, face.begin_cells(), face.end_cells()))
		return p;
	const Feature *closest = closest_to_E(edge, face);
	if(typeid(*closest) == typeid(Vertex))
		return nearest_pt(*(Vertex*)closest, edge);
	else{
		Point q;
		nearest_pts(edge, p, *(Winged*)closest, q);
		return p;
	}
}

}
}
