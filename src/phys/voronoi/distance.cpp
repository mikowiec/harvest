#include "os/compat.h"

#include "distance.h"

#include "geometry.h"

#include <iostream>
#include <limits>
#include <typeinfo>
#include <cmath>

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

void disp_vertex(const Vertex& v)
{
	cout << "vertex: (" << v.x << ", " << v.y << ", " << v.z << ")";
}

double triple(const reaper::Vector &a, const reaper::Vector &b, const reaper::Vector &c)
{
	return dot(cross(a, b), c);
}

double sign(double x){
	return x/fabs(x);
}

ClosestFeatures *close_feat_checkp(const Feature &a, const Feature &b){
	cout << "Checking dynamic type information for next closest feature candiate par." << endl;
	if(typeid(a) == typeid(Vertex) && typeid(a) == typeid(Vertex)){
		cout << "Decided on vertex-vertex." << endl;
		return vertex_vertex((Vertex&)a, (Vertex&)b);
	}
	if(typeid(a) == typeid(Vertex) && typeid(a) == typeid(Winged)){
		cout << "Decided on vertex-winged." << endl;
		return vertex_winged((Vertex&)a, (Winged&)b);
	}
	if(typeid(a) == typeid(Vertex) && typeid(a) == typeid(Face)){
		cout << "Decided on vertex-face." << endl;
		return vertex_face((Vertex&)a, (Face&)b);
	}
	if(typeid(a) == typeid(Winged) && typeid(a) == typeid(Vertex)){
		cout << "Decided on vertex-winged." << endl;
		return vertex_winged((Vertex&)b, (Winged&)a);
	}
	if(typeid(a) == typeid(Winged) && typeid(a) == typeid(Winged)){
		cout << "Decided on winged-winged." << endl;
		return winged_winged((Winged&)a, (Winged&)b);
	}
	if(typeid(a) == typeid(Winged) && typeid(a) == typeid(Face)){
		cout << "Decided on winged-face." << endl;
		return winged_face((Winged&)a, (Face&)b);
	}
	if(typeid(a) == typeid(Face) && typeid(a) == typeid(Vertex)){
		cout << "Decided on vertex-face." << endl;
		return vertex_face((Vertex&)b, (Face&)a);
	}
	if(typeid(a) == typeid(Face) && typeid(a) == typeid(Winged)){
		cout << "Decided on winged-face." << endl;
		return winged_face((Winged&)b, (Face&)b);
	}
	else{
		cout << "Decided on face-face." << endl;
		return face_face((Face&)a, (Face&)b);
	}

}

const Feature *closest_to_E(const Edge &edge, const Face &face)
{
	cout << "Finding closest feature on face to edge." << endl;
	const Feature *closest;
	double min = numeric_limits<double>::max();
	Point p, q;
	for(Face::ConstVertexIterator i = face.begin_vertices(); i != face.end_vertices(); ++i){
		p = nearest_pt(*(*i), edge);
		double test = distance(p, *(*i));
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	for(Face::ConstWingedIterator i = face.begin_wingeds(); i != face.end_wingeds(); ++i){
		nearest_pts(edge, p, *(*i), q);
		double test = distance(p, q);
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	return closest;
}

const Feature *find_closest(const Point &point, const Polyhedron &polyhedron)
{
	cout << "Finding closest feature on polyhedron to point." << endl;
	Point p;
	const Feature *closest;
	double min = numeric_limits<double>::max();
	for(Polyhedron::ConstVertexIterator i = polyhedron.begin_vertices(); i != polyhedron.end_vertices(); ++i){
		double test = distance(*(*i), point);
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	for(Polyhedron::ConstWingedIterator i = polyhedron.begin_wingeds(); i != polyhedron.end_wingeds(); ++i){
		p = nearest_pt(point, *(*i));
		double test = distance(p, point);
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	for(Polyhedron::ConstFaceIterator i = polyhedron.begin_faces(); i != polyhedron.end_faces(); ++i){
		p = nearest_pt(point, *(*i));
		double test = distance(p, point);
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	return closest;
}


const Feature *find_closest(const Edge &edge, const Polyhedron &polyhedron)
{
	cout << "Finding closest feature on polyhedron to edge." << endl;
	Point p, q;
	const Feature *closest;
	double min = numeric_limits<double>::max();
	for(Polyhedron::ConstVertexIterator i = polyhedron.begin_vertices(); i != polyhedron.end_vertices(); ++i){
		p = nearest_pt(*(*i), edge);
		double test = distance(p, *(*i));
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	for(Polyhedron::ConstWingedIterator i = polyhedron.begin_wingeds(); i != polyhedron.end_wingeds(); ++i){
		nearest_pts(edge, p, *(*i), q);
		double test = distance(p, q);
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	for(Polyhedron::ConstFaceIterator i = polyhedron.begin_faces(); i != polyhedron.end_faces(); ++i){
		p = nearest_pt(edge, *(*i));
		double test = Plane(*(*i)).distance_to(p);
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	return closest;
}

const Feature *find_closest(const Face &face, const Polyhedron &polyhedron)
{
	cout << "Finding closest feature on polyhedron to face" << endl;
	Point p;
	const Feature *closest;
	double min = numeric_limits<double>::max();
	for(Polyhedron::ConstVertexIterator i = polyhedron.begin_vertices(); i != polyhedron.end_vertices(); ++i){
		p = nearest_pt(*(*i), face);
		double test = distance(p, *(*i));
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	for(Polyhedron::ConstWingedIterator i = polyhedron.begin_wingeds(); i != polyhedron.end_wingeds(); ++i){
		p = nearest_pt(*(*i), face);
		double test = Plane(face).distance_to(p);
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	for(Polyhedron::ConstFaceIterator i = polyhedron.begin_faces(); i != polyhedron.end_faces(); ++i){
		double test = distance(*(*i), face);
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	return closest;
}

const Feature *point_face_checkp(const Point &point, const Face &face)
{
	cout << "Point-face applicability." << endl;
	const Feature *nbr = point_cell_checkp(point, face.begin_cells(), face.end_cells());
	if(!nbr){
		if(Plane(face).distance_to(point)>0)
			return nbr;
		else
			return find_closest(point, *face.coboundary());
	}
	else
		return nbr;
}

bool E_FPrism(const Edge &edge, const Face &face)
{
	cout << "Check if edge overlap face." << endl;
	double min = 0;
	double max = length(edge);
	
	for(Feature::ConstCellIterator i = face.begin_cells(); i != face.end_cells(); ++i){
		double norm = dot(edge.unit_vector(), (*i)->constraint_plane().unit_normal());

		// edge points inward from the hyperplane
		if(norm>0){
			double k = (*i)->constraint_plane().distance_to(edge.head()) / norm;
			if(k<max){
				max = k;
				if(min>max)
					return false;
			}
		}
		// edge points outward from the hyperplane
		else if(norm<0){
			double k = (*i)->constraint_plane().distance_to(edge.head()) / norm;
			if(k>min){
				min = k;
				if(max<min)
					return false;
			}
		}
		// edge is parallel to hyperplane
		else
			if((*i)->constraint_plane().distance_to(edge.head())<0)
				return false;
	}
	
	return true;
}

const Feature *closest_to_Fplane(const Face &a, const Face &b)
{
	cout << "Find closest feature of face to another face's plane" << endl;
	Plane plane(a);
	cout << "ho" << endl;
	double min = numeric_limits<double>::max();
	cout << "ho" << endl;
	const Feature *closest;
	cout << "ho" << endl;
	
	for(Face::ConstVertexIterator i = b.begin_vertices(); i != b.end_vertices(); ++i){
		cout << "hi" << endl;
		double test = plane.distance_to(*(*i));
		if(test<min){
			min = test;
			closest = *i;
		}
	}

	cout << "Done checking vertices." << endl;

	for(Face::ConstWingedIterator i = b.begin_wingeds(); i != b.end_wingeds(); ++i){
		double test = distance(*(*i), plane);
		if(test<min){
			min = test;
			closest = *i;
		}
	}
	return closest;
}

void closest_wingeds(const Face &a, Winged const *&v, const Face &b, Winged const *&w)
{
	cout << "Find two closest wingeds on two faces" << endl;
	double min = numeric_limits<double>::max();
	Point p, q;
	for(Face::ConstWingedIterator i = a.begin_wingeds(); i != a.end_wingeds(); ++i)
		for(Face::ConstWingedIterator j = b.begin_wingeds(); j != b.end_wingeds(); ++j){
			nearest_pts(*(*i), p, *(*j), q);
			double test = distance(p, q);
			if(test<min){
				min = test;
				v = *i;
				w = *j;
			}
		}
}

ClosestFeatures *winged_face(const Winged &winged, const Face &face)
{
	cout << "Winged-face applicability." << endl;
	if(dot(winged.head(), face.normal()) == dot(winged.tail(), face.normal())){
		if(E_FPrism(winged, face)){
			if(Plane(face).distance_to(winged.head())>0){
				double cp_right = triple(winged, winged.right_face()->normal(), face.normal());
				double cp_left  = triple(winged, winged.left_face()->normal(), face.normal());
				if(cp_right>=0){
					if(cp_left>=0)
						return new ClosestFeatures(&winged, &face, Plane(face).distance_to(winged.tail()));
					else
						close_feat_checkp(*winged.left_face(), face);
				} 
				else
					close_feat_checkp(*winged.right_face(), face);
			} 
			else
				close_feat_checkp(winged, *find_closest(winged, *face.coboundary()));
		} 
		else
			close_feat_checkp(winged, *closest_to_E(winged, face));
	} 
	else if(sign(dot(winged.head(), face.normal())) != 
		sign(dot(winged.tail(), face.normal())))
		close_feat_checkp(winged, *closest_to_E(winged, face));
	else {
		Point p = nearest_pt(winged.head(), face);
		Point q = nearest_pt(winged.head(), face);
		if(distance(p, winged.head())<distance(q, winged.head()))
			sub_winged_face(*winged.head_vertex(), winged, face);
		else
			sub_winged_face(*winged.tail_vertex(), winged, face);
	}
}


ClosestFeatures *sub_winged_face(const Vertex &vertex, const Winged &winged, const Face &face)
{
	const Feature *nbrb = point_cell_checkp(vertex, face.begin_cells(), face.end_cells());
	if(!nbrb){
		if(dot(vertex, face.normal())>0){
			Point p = nearest_pt(vertex, face);
			const Feature *nbra = 
				point_cell_checkp(p, vertex.begin_cells(), vertex.end_cells());
			if(!nbra)
				return new ClosestFeatures(&vertex, &face, distance(p, vertex));
			else
				return close_feat_checkp(*nbra, face);
		}
		else
			return close_feat_checkp(vertex, *find_closest(vertex, *face.coboundary()));
	}
	else
		return close_feat_checkp(winged, *closest_to_E(winged, face));
	
}


ClosestFeatures *sub_face_face(const Face &a, const Face &b)
{
  	const Feature *nbrb = closest_to_Fplane(a, b);
	if(typeid(*nbrb) == typeid(Vertex)){
		const Feature *nbra = point_face_checkp(*(Vertex*)nbrb, a);
		if(!nbra)
			return vertex_face(*(Vertex*)nbrb, a);
		else {
			const Winged *v, *w;
			closest_wingeds(a, v, b, w);
			return winged_winged(*v, *w);
		}
	}
	else if(E_FPrism(*(Winged*)nbrb, a)) {
		if(Plane(a).distance_to(((Winged*)nbrb)->head())>0)
			return winged_face(*(Winged*)nbrb, a);
		else
			return close_feat_checkp(*find_closest(*(Winged*)nbrb, *a.coboundary()), *nbrb);
	}
	else{
		const Winged *v, *w;
		closest_wingeds(a, v, b, w);
		return winged_winged(*v, *w);
	}
}

bool overlap(const Face &a, const Face &b)
{
	cout << "Check if a face projects down to another face." << endl;
	for(Face::ConstWingedIterator i = a.begin_wingeds(); i != a.end_wingeds(); ++i)
		if(E_FPrism(*(*i), b))
			return true;
	return false;
}

ClosestFeatures *face_face(const Face &a, const Face &b)
{
	cout << "Face-face applicability." << endl;
	if(fabs(dot(a.normal(), b.normal())) == 1){
		Vertex *va = *a.begin_vertices();
		if(Plane(b).distance_to(*va)>0){
			Vertex *vb = *b.begin_vertices();
			if(Plane(a).distance_to(*vb)>0)
				return new ClosestFeatures(&a, &b, distance(*va, *vb));
			else
				return close_feat_checkp(*find_closest(b, *a.coboundary()), b);
		}
		else {
			const Winged *v, *w;
			closest_wingeds(a, v, b, w);
			return winged_winged(*v, *w);
		}
	}
	else {
		const Feature *nbra = closest_to_Fplane(b, a);
		if(typeid(*nbra) == typeid(Vertex)){
			const Feature *nbrb = point_face_checkp(*(Vertex*)nbra, b);
			if(!nbrb)
				return vertex_face(*(Vertex*)nbra, b);
			else
				sub_face_face(a, b);
		} 
		else if(E_FPrism(*(Winged*)nbra, b)){
			if(Plane(b).distance_to(((Winged*)nbra)->head())>0)
				return winged_face(*(Winged*)nbra, b);
			else
				return close_feat_checkp(*(Winged*)nbra, *find_closest(*(Winged*)nbra, *b.coboundary()));
		}
		else
			sub_face_face(a, b);
	}
}

ClosestFeatures *winged_winged(const Winged &a, const Winged &b)
{
	cout << "Winged-winged applicability." << endl;
	Point p, q;
	nearest_pts(a, p, b, q);
	const Feature *nbrb = point_cell_checkp(p, b.begin_cells(), b.end_cells());
	if(!nbrb){
		const Feature *nbra = point_cell_checkp(q, a.begin_cells(), a.end_cells());
		if(!nbra)
			return new ClosestFeatures(&a, &b, distance(p, q));
		else
			return close_feat_checkp(*nbra, b);
	}
	else
		return close_feat_checkp(a, *nbrb);
}

ClosestFeatures *vertex_face(const Vertex &vertex, const Face &face)
{
	cout << "Vertex-face applicability." << endl;
	const Feature *nbrb = point_face_checkp(vertex, face);
	if(!nbrb){
		const Point nearest = nearest_pt(vertex, face);
		const Feature *nbra = 
			point_cell_checkp(nearest, vertex.begin_cells(), vertex.end_cells());
		if(!nbra)
			return new ClosestFeatures(&vertex, &face, distance(nearest, vertex));
		else
			winged_face(*dynamic_cast<const Winged*>(nbra), face);
	}
	else
		return vertex_winged(vertex, *dynamic_cast<const Winged*>(nbrb));
}


ClosestFeatures *vertex_winged(const Vertex &vertex, const Winged &winged)
{
	cout << "Vertex-winged applicability." << endl;
	const Feature *nbrb = point_cell_checkp(vertex, winged.begin_cells(), winged.end_cells());
	if(!nbrb){
		const Point nearest = nearest_pt(vertex, winged);
		const Feature *nbra = point_cell_checkp(nearest, vertex.begin_cells(), vertex.end_cells());
		if(!nbra)
			return new ClosestFeatures(&vertex, &winged, distance(vertex, nearest));
		else
			return winged_winged(*dynamic_cast<const Winged*>(nbra), winged);
	}
	else
		return close_feat_checkp(vertex, *nbrb);
}


ClosestFeatures *vertex_vertex(const Vertex &a, const Vertex &b)
{
	cout << "Vertex-vertex applicability." << endl;
	const Feature *nbrb = point_cell_checkp(a, b.begin_cells(), b.end_cells());
	if(!nbrb){
		const Feature *nbra = point_cell_checkp(b, a.begin_cells(), a.end_cells());
		if(!nbra)
			return new ClosestFeatures(&a, &b, distance(a, b));
		else
			return vertex_winged(b, *dynamic_cast<const Winged*>(nbra));
	}
	else
		return vertex_winged(a, *dynamic_cast<const Winged*>(nbrb));
}

}
}
