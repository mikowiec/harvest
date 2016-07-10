#ifndef REAPER_PHYS_FACE_H
#define REAPER_PHYS_FACE_H

#include <vector>

#include "main/types.h"
#include "utility.h"

#include "feature.h"
#include "polygon.h"

#include "stlhelper.h"

namespace reaper
{
namespace phys
{

class Polyhedron;
class Point;
class Winged;
class Edge;
class Vertex;

/// Face feature of polyhedron
class Face : public Feature, public Polygon 
{
public:
	typedef hlp::cast_iterator<std::vector<Point*>::iterator, Vertex*> VertexIterator;
	typedef hlp::cast_iterator<std::vector<Edge*>::iterator, Winged*> WingedIterator;
	typedef hlp::cast_iterator<std::vector<Edge*>::const_iterator, Winged*> ConstWingedIterator;
	typedef hlp::cast_iterator<std::vector<Point*>::const_iterator, Vertex*> ConstVertexIterator;

	typedef std::back_insert_iterator<std::vector<Edge*> > WingedInserter;
	typedef std::back_insert_iterator<std::vector<Point*> > VertexInserter;

	Face() {}

//	const Vector& normal() const { return norm; }

	VertexIterator begin_vertices() { return points.begin(); }
	VertexIterator end_vertices()   { return points.end();   } 

	WingedIterator begin_wingeds() { return edges.begin(); }
	WingedIterator end_wingeds()   { return edges.end();   }

	ConstWingedIterator begin_wingeds() const { return edges.begin(); } 
	ConstWingedIterator end_wingeds()   const { return edges.end();   }

	ConstVertexIterator begin_vertices() const { return points.begin(); }
	ConstVertexIterator end_vertices()   const { return points.end();   }

	WingedInserter winged_inserter() { return std::back_inserter(edges); }
	VertexInserter vertex_inserter() { return std::back_inserter(points);  }

	const Polyhedron *coboundary() const { return cobnd; }
	Polyhedron *&coboundary() { return cobnd; }

//	const Vector &Feature::normal() const { return Polygon::normal(); }
//	Vector &Feature::normal() { return Polygon::normal(); }
private:
/*  	std::vector<Vertex*> verts;   */
/*  	std::vector<Winged*> wingeds;     */
/*  	std::vector<Winged*> left_wingeds;  */
/*  	std::vector<Winged*> right_wingeds; */

	Polyhedron *cobnd;	     
};

}
}
#endif


