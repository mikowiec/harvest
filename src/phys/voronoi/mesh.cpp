#include "os/compat.h"

#include <iostream>
#include <algorithm>
#include <utility>

#include "mesh.h"

#include "main/types.h"
#include "utility.h"

#include "plane.h"
#include "vertex.h"
#include "winged.h"
#include "face.h"
#include "polyhedron.h"
#include "cell.h"

namespace reaper
{
namespace phys
{

using namespace std;

void put_vertex(const Vertex &v)
{
	cout << "(" << v.x << ", " << v.y << ", " << v.z << ")" << endl;
}

void put_winged(const Winged &w)
{
	cout << "<(" << w.tail().x << ", " << w.tail().y << ", " << w.tail().z << "), " << 
		"("  << w.head().x << ", " << w.head().y << ", " << w.head().z << ")>" <<  endl;
}

Polyhedron *cube(double w, double h, double d)
{
	cout << "Begin unit cube" << endl;
	double vert_coords[][3] = { {-0.5*w,  0.5*h, -0.5*d}, { 0.5*w,  0.5*h, -0.5*d},
				    { 0.5*w,  0.5*h,  0.5*d}, {-0.5*w,  0.5*h,  0.5*d},
				    {-0.5*w, -0.5*h, -0.5*d}, { 0.5*w, -0.5*h, -0.5*d},
				    { 0.5*w, -0.5*h,  0.5*d}, {-0.5*w, -0.5*h,  0.5*d} };
        
        int edge_verts[12][2] = { {0, 1}, {1, 2}, {2, 3}, {3, 0},
				  {1, 5}, {2, 6}, {3, 7}, {0, 4},
				  {4, 5}, {5, 6}, {6, 7}, {7, 4} };
	
//          int face_verts[24] = { 0, 1, 2, 3,   1, 0, 4, 5,
//  			       2, 1, 5, 6,   0, 3, 7, 4,
//  			       3, 2, 6, 7,   7, 6, 5, 4 };

        int face_verts[24] = { 3, 2, 1, 0,  5, 4, 0, 1,
			       6, 5, 1, 2,  4, 7, 3, 0,
			       7, 6, 2, 3,  4, 5, 6, 7};
	
        int num_face_verts[6] = {4, 4, 4, 4, 4, 4};

	Mesh mesh;

	for(int i=0; i<8; ++i)
		mesh.vertices.push_back(Point(vert_coords[i][0], vert_coords[i][1], vert_coords[i][2]));
	
	for(int i=0; i<12; ++i)
		mesh.wingeds.push_back(make_pair(edge_verts[i][0], edge_verts[i][1]));

	int c = 0;

	for(int i=0; i<6; ++i){
		mesh.faces.push_back(vector<int>());
		for(int j=0; j<4; ++j, ++c)
			mesh.faces.rbegin()->push_back(face_verts[c]);
	}

	cout << "Building.. " << endl;

        return build_polyhedron(mesh, 1.0);
}


/// Assemble polyhedron from mesh
Polyhedron* build_polyhedron(const Mesh& mesh, double alpha)
{
	Polyhedron *object = new Polyhedron();

	Polyhedron::VertexInserter vi = object->vertex_inserter(); 

	cout << "Creating vertices.. " << endl;

	for(vector<Point>::const_iterator i = mesh.vertices.begin(); i != mesh.vertices.end(); ++i)
		*vi++ = new Vertex(*i);

	Polyhedron::WingedInserter wi = object->winged_inserter();

	cout << "Creating wingeds.. " << endl;

	for(vector<pair<int, int> >::const_iterator i = mesh.wingeds.begin(); i != mesh.wingeds.end(); ++i){
		Vertex *tail = object->begin_vertices()[i->first];
		Vertex *head = object->begin_vertices()[i->second];
		Winged *winged = new Winged(tail, head);
		*tail->winged_inserter() = winged;
		*head->winged_inserter() = winged;
		*wi++ = winged;
	}

	Polyhedron::FaceInserter fi = object->face_inserter();

	cout << "Creating faces.. " << endl;

	for(vector<std::vector<int> >::const_iterator i = mesh.faces.begin(); i != mesh.faces.end(); ++i){
		cout << "New face" << endl;
		Face *face = new Face();
		face->coboundary() = object;
		Face::VertexInserter fvi = face->vertex_inserter();
		for(vector<int>::const_iterator j = i->begin(); j != i->end(); ++j){
			*fvi++ = object->begin_vertices()[*j];
			cout << "Vertex # " << *j << endl;
		}
		face->normal() = norm(cross(*(*(face->begin_points()+2)) - *(*(face->begin_points()+1)), 
							  *(*face->begin_points())     - *(*(face->begin_points()+1))));
		*fi++ = face;
	}

	typedef Polyhedron::FaceIterator FI;

	cout << "Connecting winged/faces.. " << endl;

	for(FI face=object->begin_faces(); face!=object->end_faces(); ++face){
		cout << "New face" << endl;
		Face::WingedInserter fwi = (*face)->winged_inserter();
		typedef Face::VertexIterator VI;

		VI previous = --(*face)->end_vertices();
		for(VI current=(*face)->begin_vertices(); current!=(*face)->end_vertices(); ++current){

			cout << "current vertex is ";
			put_vertex(*(*current));
			cout << "previous vertex is ";
			put_vertex(*(*previous));

			typedef Vertex::WingedIterator WI;

			for(WI winged=(*current)->begin_wingeds(); winged != (*current)->end_wingeds(); ++winged){
				
				if(*current == (*winged)->head_vertex() && *previous == (*winged)->tail_vertex()){
					cout << "left edge: ";
					put_winged(*(*winged)); 
					(*winged)->left_face() = *face;
					*fwi++ = *winged;
				} else if (*current == (*winged)->tail_vertex() && *previous == (*winged)->head_vertex()){
					cout << "right edge: ";
					put_winged(*(*winged)); 
					(*winged)->right_face() = *face;
					*fwi++ = *winged;
				}
			}
			previous = current;
		}
	}

	typedef Polyhedron::VertexIterator VI;

	cout << "Creating vertex cells.. " << endl;

	Feature::CellInserter ci = object->cell_inserter(); 
	for(VI vertex=object->begin_vertices(); vertex!=object->end_vertices(); ++vertex){
		typedef Vertex::WingedIterator WI;
		Feature::CellInserter vci = (*vertex)->cell_inserter();

		for(WI winged=(*vertex)->begin_wingeds(); winged!=(*vertex)->end_wingeds(); ++winged)
			if(*vertex==(*winged)->head_vertex())
				(*vertex)->normal() = (*vertex)->normal() +  (*winged)->unit_vector();
			else if(*vertex==(*winged)->tail_vertex())
				(*vertex)->normal() = (*vertex)->normal() -  (*winged)->unit_vector();
		(*vertex)->normal() = normalize3((*vertex)->normal());

		for(WI winged=(*vertex)->begin_wingeds(); winged!=(*vertex)->end_wingeds(); ++winged){
			Vector normal;

			if(*vertex==(*winged)->head_vertex()){
				normal = (*winged)->unit_vector();
			}
			else if(*vertex==(*winged)->tail_vertex()){
				normal = -(*winged)->unit_vector();
			}
			else
				cout << "inte bra" << endl;

			Vector up = norm((*vertex)->normal() -  dot(normal, (*vertex)->normal())*normal);
			Vector side = cross(normal, up);

			Cell *cell = new Cell(Plane(Matrix4(alpha, norm(side))*normal, *(*vertex)), *winged);
			
			*ci++  = cell;
			*vci++ = cell;
		}
	} 


	cout << "Creating winged cells.. " << endl;

	typedef Polyhedron::WingedIterator WI;

	for(WI winged = object->begin_wingeds(); winged!=object->end_wingeds(); ++winged){
		Feature::CellInserter wci = (*winged)->cell_inserter();
		
		Cell *cell;
		Vector normal;
		
		normal = cross((*winged)->right_face()->normal(), (*winged)->unit_vector());
		cell = new Cell(Plane(Matrix4(alpha, normalize3(*(*winged)))*normal, *(*winged)->tail_vertex()), (*winged)->right_face());
		*ci++  = cell;
		*wci++ = cell; 

		(*winged)->normal() = cell->constraint_plane().unit_normal();


		normal = cross((*winged)->unit_vector(), (*winged)->left_face()->normal());
		cell =  new Cell(Plane(Matrix4(alpha, norm(-*(*winged)))*normal, *(*winged)->tail_vertex()), (*winged)->left_face());
		*ci++  = cell;
		*wci++ = cell; 

		(*winged)->normal() = norm((*winged)->normal() + 
						 cell->constraint_plane().unit_normal());
		
		normal = (*winged)->unit_vector();
		cell = new Cell(Plane(Matrix4(alpha, norm(cross_product(normal, (*winged)->normal())))*normal, *(*winged)->tail_vertex()), (*winged)->tail_vertex());
		*ci++  = cell;
		*wci++ = cell; 

		cell = new Cell(Plane(Matrix4(alpha, norm(cross_product((*winged)->normal(), normal)))*(-normal), *(*winged)->head_vertex()), (*winged)->head_vertex());
		*ci++  = cell;
		*wci++ = cell; 

	}

	typedef Polyhedron::FaceIterator FI;

	cout << "Creating face cells.. " << endl;

	for(FI face = object->begin_faces(); face != object->end_faces(); ++face){
		int c = 0;
		typedef Face::WingedIterator WI;
		Feature::CellInserter fci = (*face)->cell_inserter();
		for(WI winged = (*face)->begin_wingeds(); winged != (*face)->end_wingeds(); ++winged, ++c){
			Vector normal;
			double x = 1;
			if((*winged)->right_face() == *face){
				x = -1;
				normal = cross((*winged)->unit_vector(), (*face)->normal());
			}
			else if((*winged)->left_face() == *face)
				normal = cross((*face)->normal(), (*winged)->unit_vector());
			else
				cout << "KNAAAS!" << endl;
			Cell* cell = new Cell(Plane(Matrix4(alpha, normalize3(x*(*(*winged))))*normal, (*winged)->tail()), *winged);
			*ci++  = cell;
			*fci++ = cell;
		}
		cout << c << " cells for face" << endl;

	}

	cout << "Done building" << endl;

	return object;
}


}
}







