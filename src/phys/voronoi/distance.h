#include "main/types.h"

#include <utility>

#include "point.h"
#include "plane.h"

namespace reaper
{
namespace phys
{

class Feature;
class Edge;
class Face;
class Polyhedron;
class Plygon;
class Vertex;
class Winged;	

class ClosestFeatures : public std::pair<const Feature*, const Feature*>
{
public:
	ClosestFeatures(const Feature *first, const Feature *second, double dist) : 
		std::pair<const Feature*, const Feature*>(first, second), distance(dist) {}

	const double distance;
};

template<class For>
const Feature *point_cell_checkp(const Point &p, For begin, For end)
{
	double min = 0;
	const Feature *nbr = 0;
	for(For i = begin; i != end; ++i){
		double test = (*i)->constraint_plane().distance_to(p);
		if(test<min){
			min = test;
			nbr = (*i)->neighbour();
		}
	}
	return nbr;
}

ClosestFeatures *close_feat_checkp(const Feature &a, const Feature &b);

const Feature *closest_to_E(const Edge &edge, const Face &face);

const Feature *find_closest(const Edge &edge, const Polyhedron &polyhedron);

const Feature *find_closest(const Point &point, const Polyhedron &polyhedron);

const Feature *point_face_checkp(const Point &point, const Face &face);

bool E_FPrism(const Edge &edge, const Face &face);

const Feature *closest_to_Fplane(const Face &a, const Face &b);

void closest_wingeds(const Face &a, Winged const *&v, const Face &b, Winged const *&w);

ClosestFeatures *face_face(const Face &a, const Face &b);

ClosestFeatures *sub_face_face(const Face &a, const Face &b);

ClosestFeatures *winged_face(const Winged &winged, const Face &face);

ClosestFeatures *sub_winged_face(const Vertex &vertex, const Winged &winged, const Face &face);

ClosestFeatures *winged_winged(const Winged &a, const Winged &b);

ClosestFeatures *vertex_face(const Vertex &vertex, const Face &face);

ClosestFeatures *vertex_winged(const Vertex &vertex, const Winged &winged);

ClosestFeatures *vertex_vertex(const Vertex &a, const Vertex &b);

}
}
