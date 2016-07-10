#ifndef REAPER_PHYS_WINGED_H
#define REAPER_PHYS_WINGED_H

#include "main/types.h"

#include "vertex.h"
#include "feature.h"
#include "edge.h"

namespace reaper
{
namespace phys
{

class Mesh;
class Polyhedron;
class Face;

class Winged : public Feature, public Edge
{
public:
	Winged() : Edge() {}

	Winged(Vertex *tail, Vertex *head) :
	  Edge(tail, head), head_pt(head) {}

	Winged(Vertex *tail, Vertex *head, Face *fleft, Face *fright) :
	  Edge(tail, head), fright(fright), fleft(fleft), head_pt(head) {}

//  	/// Get head boundary vertex
//  	const Vertex &head_vertex() const { return *static_cast<Vertex*>(tail_pt); }
//  	/// Get tail boundary vertex
//  	const Vertex &tail_vertex() const { return *static_cast<Vertex*>(head_pt); }

	/// Get head boundary vertex
	const Vertex *head_vertex() const { return static_cast<Vertex*>(head_pt); }
	/// Get tail boundary vertex
	const Vertex *tail_vertex() const { return static_cast<Vertex*>(tail_pt); }

	/// Get head boundary vertex
	Vertex *head_vertex() { return static_cast<Vertex*>(head_pt); }
	/// Get tail boundary vertex
	Vertex *tail_vertex() { return static_cast<Vertex*>(tail_pt); }

	const Face *right_face() const { return fright; }
	const Face *left_face()  const { return fleft; }

	Face *&right_face() { return fright; }
	Face *&left_face()  { return fleft; }

	const Vector &normal() const { return norm; }
	Vector &normal() { return norm; }

	Winged &operator=(const Vector4 &v) { Vector4::operator=(v); unit_norm = normalize3(v); return *this; }

private:
	Face *fright, *fleft;
	Vertex *head_pt;

	Vector norm;
};

}
}

#endif
