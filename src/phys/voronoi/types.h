#ifndef REAPER_PHYS_TYPES_H
#define REAPER_PHYS_TYPES_H


#include <vector>
#include <cmath>

#include "os/compat.h"
#include "main/types.h"
#include "main/types.h"

namespace reaper
{
namespace phys
{
class Cell;
class Edge;
class Face;
class Polyhedron;

/// Low-level data structure for meshes
struct Mesh {
	double **vert_coords;
	int num_verts;
	int **edge_verts;
	int num_edges;
	int *face_verts;
	int *num_face_verts; 
	int num_faces;
};

Polyhedron* build_polyhedron(const Mesh& m);

/// Base of polyhedron feature object  
class Feature
{
public:
	friend Polyhedron* build_polyhedron(const Mesh& m);
	typedef std::vector<Cell*>::const_iterator CellIterator;

	Feature() {}; 

	/// Get iterator pointing to the first of cell of feature Voronoi region
	CellIterator begin_cells() const { return cells.begin(); }
	/// Get iterator pointing beyond the last cell of feature Voronoi region
	CellIterator end_cells()   const { return cells.end();   }

	virtual ~Feature() {}
protected:
	/// Add new cell to feature voronoi region
	void add_cell(Cell *cell)
		{ cells.push_back(cell); };

	std::vector<Cell*> cells; ///< Voronoi region of feature
	
};

/// Vertex feature of polyhedron
class Vertex : public Feature, public Point 
{
public:
	friend Polyhedron* build_polyhedron(const Mesh& m);

	typedef std::vector<Edge*>::const_iterator EdgeIterator;

	Vertex() : Point() {}
	
	Vertex(double x, double y, double z) : Point(x, y, z) {}
//	Vertex(double *dv) : Point(dv[0], dv[1], dv[2]) {}

	/// Get iterator pointing to the first of coboundary edges
	EdgeIterator begin_edges() { return edges.begin(); }
	/// Get iterator pointing beyond the last oboundary edge
	EdgeIterator end_edges()   { return edges.end();   }
private:
	std::vector<Edge*> edges; ///< Coboundery edges of vertex
};


/// Edge feature of polyhedron
class Edge : public Feature 
{
public:
	friend Polyhedron* build_polyhedron(const Mesh& m);

	Edge(Vertex* tail, Vertex* head);

	/// Get right coboundary face
	const Face &right_face() const { return *fright; }
	/// Get left coboundary face
	const Face &left_face()  const { return *fleft; }

	/// Get tail boundary vertex
	const Vertex &head() const { return *vhead; }
	/// Get tail boundary vertex
	const Vertex &tail() const { return *vtail; }

	/// Get unit vector in edge direction
	const Vector& unit_vector() const { return vector; };
private:
	Vertex *vhead, *vtail;
	Face *fright, *fleft;	
				
	Vector vector;		
};

/// Face feature of polyhedron
class Face : public Feature 
{
public:
	friend Polyhedron* build_polyhedron(const Mesh& m);

	typedef std::vector<Edge*>::const_iterator EdgeIterator;
	typedef std::vector<Vertex*>::const_iterator VertexIterator;

	Face() {}

	const Vector& normal() const { return norm; }

	/// Get iterator pointing to first of boundary edges for which face is right coboundary
	EdgeIterator begin_right_edges() const { return right_edges.begin(); }
	/// Get iterator pointing beyond the last boundary edges for which face is right coboundary
	EdgeIterator end_right_edges()   const { return right_edges.end();   }

	/// Get iterator pointing to first of boundary edges for which face is left coboundary
	EdgeIterator begin_left_edges() const { return left_edges.begin(); }
	/// Get iterator pointing beyond the last boundary edges for which face is left coboundary
	EdgeIterator end_left_edges()   const { return left_edges.end();   }

	/// Get iterator pointing to first of all boundary edges
	EdgeIterator begin_edges() const { return edges.begin(); }
	/// Get iterator pointing beyond the last of all boundary edges
	EdgeIterator end_edges()   const { return edges.end();   }

	/// Get iterator pointing to first of boundary vertices
	VertexIterator begin_vertices() const { return verts.begin(); }
	/// Get iterator pointing beyond last of boundary vertices
	VertexIterator end_vertices()   const { return verts.end();   }
private:
	std::vector<Vertex*> verts;  
	std::vector<Edge*> edges;    
	std::vector<Edge*> left_edges; 
	std::vector<Edge*> right_edges;
	Vector norm;                 
//	Polyhedron *cobnd;	     
};

/// A plane in 3d-space
class Plane 
{
public:
	Plane(const Vector& norm, const Vertex& p);

	const Vector& unit_normal() const { return unit_norm; }

	/// Get distance from plane to a point
	double distance(const Point& p) const { return dot_product4(p, unit_norm); };
private:
	Vector unit_norm;
};


/// Half-space cell of Voronoi region
/**
 * A Voronoi region is an infinit partition of space, formed from the
 * intersection of half-spaces.  A cell is such a half-space. 
 */
class Cell 
{
public:
	Cell(Plane cplane, Feature* neighbr) :
		cplane(cplane), neighbr(neighbr) {};
	/// Get boundary plane of half-space
	const Plane& constraint_plane() const { return cplane; };
	/// Get neighbouring feature whose Voronoi region share boundary plane with this cell
	const Feature &neighbour() const { return *neighbr; };

private:
	Plane cplane;
	Feature *neighbr; ///< Next feature if this applicability test fails

};

/// Convex polygon object with Voronoi space partitioning.
class Polyhedron
{
public:
	friend Polyhedron* build_polyhedron(const Mesh& m);

	typedef std::vector<Edge*>::const_iterator EdgeIterator;
	typedef std::vector<Vertex*>::const_iterator VertexIterator;
	typedef std::vector<Face*>::const_iterator FaceIterator;
	typedef std::vector<Cell*>::const_iterator CellIterator;

	Polyhedron() {};

	/// Get iterator pointing to the first vertex of polyhedron
	VertexIterator begin_vertices() { return verts.begin(); }
	/// Get iterator pointing beyond the last vertex of polyhedron
	VertexIterator end_vertices()   { return verts.end();   }

	/// Get iterator pointing to the first edge of polyhedron
	EdgeIterator begin_edges() { return edges.begin(); }
	/// Get iterator pointing beyond the last edge of polyhedron
	EdgeIterator end_edges()   { return edges.end();   }

	/// Get iterator pointing to the first cell of polyhedron Voronoi regions
	CellIterator begin_cells() const { return cells.begin(); }
	/// Get iterator pointing beyond the last cell of polyhedron Voronoi regions
	CellIterator end_cells()   const { return cells.end();   }

	/// Get iterator pointing to the first face of polyhedron
	FaceIterator begin_faces() const { return faces.begin(); }
	/// Get iterator pointing beyond the last face of polyhedron
	FaceIterator end_faces()   const { return faces.end();   };

private:
	std::vector<Vertex*> verts; 
	std::vector<Edge*> edges;  
	std::vector<Face*> faces;
	std::vector<Cell*> cells;

	Vector rot;
	Vector pos;
};

}
}
#endif

