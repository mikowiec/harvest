#include "os/compat.h"

#include <iostream>

#include "polyhedron.h"
#include "vertex.h"
#include "winged.h"
#include "face.h"
#include "cell.h"

namespace reaper
{
namespace phys
{

void Polyhedron::matrix_mult(const Matrix4 &m)
{
	for(VertexIterator i = begin_vertices(); i != end_vertices(); ++i){
		*(*i) = m*(*(*i));
		(*i)->normal().w = 0;
		(*i)->normal() = m*(*i)->normal();
		if((*i)->w != 1)
			std::cout << "hmmm" << std::endl;
		if((*i)->normal().w != 0)
			std::cout << (*i)->normal().w << std::endl;

	}

    	for(WingedIterator i = begin_wingeds(); i != end_wingeds(); ++i){
		(*i)->w = 0;
    		*(*i) = m*(*(*i));
		(*i)->normal().w = 0;
    		(*i)->normal() = m*(*i)->normal();
    	}

	for(FaceIterator i = begin_faces(); i != end_faces(); ++i){
		(*i)->normal().w = 0;
		(*i)->normal() = m*(*i)->normal();
	}

  	for(CellIterator i = begin_cells(); i != end_cells(); ++i){
		Vector normal = (*i)->constraint_plane().unit_normal();
		normal.w = 0;
  		(*i)->constraint_plane() = Plane(m*normal, m*(*i)->constraint_plane().point());
	}

}

Polyhedron::ConstVertexIterator Polyhedron::begin_vertices() const
{
	return verts.begin();
}

Polyhedron::ConstVertexIterator Polyhedron::end_vertices() const
{
	return verts.end();
}

Polyhedron::ConstWingedIterator Polyhedron::begin_wingeds() const
{
	return wingeds.begin();
}

Polyhedron::ConstWingedIterator Polyhedron::end_wingeds() const
{
	return wingeds.end();
}

Polyhedron::ConstCellIterator Polyhedron::begin_cells() const
{
	return cells.begin();
}

Polyhedron::ConstCellIterator Polyhedron::end_cells() const
{
	return cells.end();
}

Polyhedron::ConstFaceIterator Polyhedron::begin_faces() const
{
	return faces.begin();
}

Polyhedron::ConstFaceIterator Polyhedron::end_faces() const
{
	return faces.end();
}

Polyhedron::VertexIterator Polyhedron::begin_vertices()
{
	return verts.begin();
}

Polyhedron::VertexIterator Polyhedron::end_vertices() 
{
	return verts.end();
}

Polyhedron::WingedIterator Polyhedron::begin_wingeds() 
{
	return wingeds.begin();
}

Polyhedron::WingedIterator Polyhedron::end_wingeds() 
{
	return wingeds.end();
}

Polyhedron::CellIterator Polyhedron::begin_cells() 
{
	return cells.begin();
}

Polyhedron::CellIterator Polyhedron::end_cells() 
{
	return cells.end();
}

Polyhedron::FaceIterator Polyhedron::begin_faces()
{
	return faces.begin();
}

Polyhedron::FaceIterator Polyhedron::end_faces()
{
	return faces.end();
}


}
}
