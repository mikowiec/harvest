#ifndef REAPER_PHYS_POLYHEDRON_H
#define REAPER_PHYS_POLYHEDRON_H

#include <vector>

#include "main/types.h"
#include "utility.h"

namespace reaper
{
namespace phys
{

class Cell;
class Vertex;
class Winged;
class Face;

/// Convex polygon object with Voronoi space partitioning.
class Polyhedron
{
public:
	typedef std::vector<Winged*>::iterator WingedIterator;
	typedef std::vector<Vertex*>::iterator VertexIterator;
	typedef std::vector<Face*>::iterator FaceIterator;
	typedef std::vector<Cell*>::iterator CellIterator;

	typedef std::vector<Winged*>::const_iterator ConstWingedIterator;
	typedef std::vector<Vertex*>::const_iterator ConstVertexIterator;
	typedef std::vector<Face*>::const_iterator ConstFaceIterator;
	typedef std::vector<Cell*>::const_iterator ConstCellIterator;

	typedef std::back_insert_iterator<std::vector<Winged*> > WingedInserter;
	typedef std::back_insert_iterator<std::vector<Vertex*> > VertexInserter;
	typedef std::back_insert_iterator<std::vector<Face*> > FaceInserter;
	typedef std::back_insert_iterator<std::vector<Cell*> > CellInserter;

	//	Polyhedron() : transform(true) {}

	VertexIterator begin_vertices();
	VertexIterator end_vertices();

	WingedIterator begin_wingeds();
	WingedIterator end_wingeds();

	CellIterator begin_cells();
	CellIterator end_cells();

	FaceIterator begin_faces();
	FaceIterator end_faces();

	ConstVertexIterator begin_vertices() const;
	ConstVertexIterator end_vertices()   const;

	ConstWingedIterator begin_wingeds() const;
	ConstWingedIterator end_wingeds()   const;

	ConstCellIterator begin_cells() const;
	ConstCellIterator end_cells()   const;

	ConstFaceIterator begin_faces() const;
	ConstFaceIterator end_faces()   const;

	WingedInserter winged_inserter() { return std::back_inserter(wingeds); }
	VertexInserter vertex_inserter() { return std::back_inserter(verts); }
	FaceInserter face_inserter() { return std::back_inserter(faces); }
	CellInserter cell_inserter() { return std::back_inserter(cells); }


	void matrix_mult(const Matrix4 &m);
private:
	std::vector<Vertex*> verts; 
	std::vector<Winged*> wingeds;  
	std::vector<Face*> faces;
	std::vector<Cell*> cells;
};

}
}
#endif
