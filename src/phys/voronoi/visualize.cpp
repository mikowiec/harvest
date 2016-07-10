#include "os/compat.h"

#include <algorithm>
#include <iostream>
#include <memory>

#include "visualize.h"

#include "GL/gl.h"

#include "main/types.h"
#include "main/types.h"
#include "utility.h"

#include "cell.h"
#include "edge.h"
#include "face.h"
#include "winged.h"
#include "vertex.h"
#include "polyhedron.h"

namespace reaper
{
namespace phys
{

using namespace std;

void visualize(const Polygon &p)
{
	typedef Polygon::ConstPointIterator PI;
	glBegin(GL_POLYGON);
	glNormal3fv(p.normal());
	for(PI i=p.begin_points(); i != p.end_points(); ++i){
		glVertex3fv(*(*i));
	}
	glEnd();
}

void visualize(const Face &f)
{
	visualize((const Polygon&)f);
}

void visualize(const Polyhedron &p)
{
//	find_if(it, it, pred)

	glPushMatrix();

	//	glMultMatrixf(p.transform);
	
	GLfloat red_mat[]   = { 1.0, 0.5, 0.5, 1.0 };
	GLfloat blue_mat[]   = { 0.5, 0.5, 1.0, 0.3 };
	GLfloat green_mat[]   = { 0.5, 1.0, 0.5, 0.3 };
	GLfloat yellow_mat[]   = { 0.5, 1.0, 1.0, 0.3 };
	GLfloat specular[]  = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat shininess[] = { 50.0 };
	
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLenum planes[] = { GL_CLIP_PLANE0,  
			    GL_CLIP_PLANE1, 
			    GL_CLIP_PLANE2, 
			    GL_CLIP_PLANE3,
	                    GL_CLIP_PLANE4,
			    GL_CLIP_PLANE5 };

	GLenum *plane;



	typedef Polyhedron::ConstFaceIterator FI;
	for(FI i=p.begin_faces(); i != p.end_faces(); ++i){
		plane = planes;
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
		visualize(*(*i));
		typedef Feature::ConstCellIterator CI;
		for(CI j=(*i)->begin_cells(); j != (*i)->end_cells(); ++j){
			glClipPlane(*plane, (*j)->constraint_plane());
			glEnable(*plane++);
		}
		plane = planes;
		for(CI j=(*i)->begin_cells(); j != (*i)->end_cells(); ++j){
			glDisable(*plane);
			Polygon polygon;
			polygon.normal() = (*j)->constraint_plane().unit_normal();
			Polygon::PointInserter pi = polygon.point_inserter();
			Winged *nbr = (Winged*)(*j)->neighbour();
			if(dot_product3(cross_product((*i)->normal(), nbr->unit_vector()), 
				(*j)->constraint_plane().unit_normal())>0){
				// left winged
				Vector up = cross_product(nbr->unit_vector(), (*j)->constraint_plane().unit_normal());
				*pi++ = new Point(nbr->head());
				*pi++ = new Point(nbr->tail()); 
				*pi++ = new Point(nbr->tail() + 7*up);
				*pi   = new Point(nbr->head() + 7*up);
			}
			else{
				// right winged
				Vector up = cross_product((*j)->constraint_plane().unit_normal(), nbr->unit_vector());
				*pi++ = new Point(nbr->head());
				*pi++ = new Point(nbr->head() + 7*up);
				*pi++ = new Point(nbr->tail() + 7*up);
				*pi   = new Point(nbr->tail()); 
			}
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue_mat);
			visualize(polygon);
			glEnable(*plane++);

			for(Polygon::PointIterator k = polygon.begin_points(); k != polygon.end_points(); ++k)
				delete(*k);
		}
		plane = planes;
		for(CI j=(*i)->begin_cells(); j != (*i)->end_cells(); ++j){
			glDisable(*plane++);
		}
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green_mat);

	for(Polyhedron::ConstWingedIterator i = p.begin_wingeds(); i != p.end_wingeds(); ++i){
		plane = planes;
		const Cell *tail_cell, *head_cell, *fleft_cell, *fright_cell;
		for(Feature::ConstCellIterator j = (*i)->begin_cells(); j != (*i)->end_cells(); ++j){
			if((*j)->neighbour() == (*i)->tail_vertex()){
				tail_cell = *j;
				if(dot_product3(tail_cell->constraint_plane().unit_normal(), *(*i))<0)
					std::cout << "BAD" << endl;
				glClipPlane(GL_CLIP_PLANE0, (*j)->constraint_plane());
				glEnable(GL_CLIP_PLANE0);
			}
			else if((*j)->neighbour() == (*i)->head_vertex()){
				head_cell = *j;
				if(dot_product3(head_cell->constraint_plane().unit_normal(), *(*i))>0)
					std::cout << "BAD" << endl;
				glClipPlane(GL_CLIP_PLANE1, (*j)->constraint_plane());
				glEnable(GL_CLIP_PLANE1);
			}
			else if((*j)->neighbour() == (*i)->left_face()){
				fleft_cell = *j;
				glClipPlane(GL_CLIP_PLANE2, (*j)->constraint_plane());
				glEnable(GL_CLIP_PLANE2);
			}
			else if((*j)->neighbour() == (*i)->right_face()){
				fright_cell = *j;
				glClipPlane(GL_CLIP_PLANE3, (*j)->constraint_plane());
				glEnable(GL_CLIP_PLANE3);
			}
			else
				std::cout << "SUPER KNAS!" << endl;
		}

		Vector a    = cross_product(tail_cell->constraint_plane().unit_normal(), fright_cell->constraint_plane().unit_normal());
		Vector b    = cross_product(fleft_cell->constraint_plane().unit_normal(), tail_cell->constraint_plane().unit_normal());
		Vector up   = normalize3(a+b);
		Vector side = normalize3(cross_product(up, *(*i)));
		
		Polygon *polygon = new Polygon();
		Polygon::PointInserter pi = polygon->point_inserter();
		*pi++ = new Point((*i)->tail() - 5*side);
		*pi++ = new Point((*i)->tail() + 5*side);
		*pi++ = new Point((*i)->tail() + 5*side + 5*up);
		*pi   = new Point((*i)->tail() - 5*side + 5*up);
		polygon->normal() = tail_cell->constraint_plane().unit_normal();
		glDisable(GL_CLIP_PLANE0);
		visualize(*polygon);
		glEnable(GL_CLIP_PLANE0);

		for(Polygon::PointIterator k = polygon->begin_points(); k != polygon->end_points(); ++k)
			delete(*k);
		delete(polygon);

		a = cross_product(fright_cell->constraint_plane().unit_normal(), head_cell->constraint_plane().unit_normal());
		b = cross_product(head_cell->constraint_plane().unit_normal(), fleft_cell->constraint_plane().unit_normal());
		up = normalize3(a+b);
		side = normalize3(cross_product(-*(*i), up));

		polygon = new Polygon();
		pi = polygon->point_inserter();
		*pi++ = new Point((*i)->head() + 5*side);
		*pi++ = new Point((*i)->head() - 5*side);
		*pi++ = new Point((*i)->head() - 5*side + 5*up);
		*pi   = new Point((*i)->head() + 5*side + 5*up);
		polygon->normal() = head_cell->constraint_plane().unit_normal();
		glDisable(GL_CLIP_PLANE1);
		visualize(*polygon);
		glEnable(GL_CLIP_PLANE1);

		for(Polygon::PointIterator k = polygon->begin_points(); k != polygon->end_points(); ++k)
			delete(*k);
		delete(polygon);

		up = normalize3(cross_product(fleft_cell->constraint_plane().unit_normal(), *(*i)));
		polygon = new Polygon();
		polygon->normal() = fleft_cell->constraint_plane().unit_normal();
		pi = polygon->point_inserter();
		
		*pi++ = new Point(*(*i)->tail_vertex());
		*pi++ = new Point(*(*i)->head_vertex());
		*pi++ = new Point(*(*i)->head_vertex() + 7*up);
		*pi   = new Point(*(*i)->tail_vertex() + 7*up);
		
		glDisable(GL_CLIP_PLANE2);
		visualize(*polygon);
		glEnable(GL_CLIP_PLANE2);

		for(Polygon::PointIterator k = polygon->begin_points(); k != polygon->end_points(); ++k)
			delete(*k);
		delete(polygon);

		up = normalize3(cross_product(*(*i), fright_cell->constraint_plane().unit_normal()));
		polygon = new Polygon();
		polygon->normal() = fright_cell->constraint_plane().unit_normal();
		pi = polygon->point_inserter();
		
		*pi++ = new Point(*(*i)->head_vertex());
		*pi++ = new Point(*(*i)->tail_vertex());
		*pi++ = new Point(*(*i)->tail_vertex() + 7*up);
		*pi   = new Point(*(*i)->head_vertex() + 7*up);
		
		glDisable(GL_CLIP_PLANE3);
		visualize(*polygon);
		glEnable(GL_CLIP_PLANE3);

		for(Polygon::PointIterator k = polygon->begin_points(); k != polygon->end_points(); ++k)
			delete(*k);
		delete(polygon);

		plane = planes;

		for(Feature::ConstCellIterator j = (*i)->begin_cells(); j != (*i)->end_cells(); ++j){
			glDisable(*plane++);
		}
		
	}

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow_mat);

	for(Polyhedron::ConstVertexIterator i = p.begin_vertices(); i != p.end_vertices(); ++i){
		plane = planes;
		for(Feature::ConstCellIterator j = (*i)->begin_cells(); j != (*i)->end_cells(); ++j){
			glClipPlane(*plane, (*j)->constraint_plane());
			glEnable(*plane++);			
		}
		plane = planes;
		for(Feature::ConstCellIterator j = (*i)->begin_cells(); j != (*i)->end_cells(); ++j){
			glDisable(*plane);
			Vector up = normalize3((*i)->normal() - dot_product3((*j)->constraint_plane().unit_normal(), (*i)->normal())*(*j)->constraint_plane().unit_normal());
			Vector side = cross_product(up, (*j)->constraint_plane().unit_normal());
			Polygon polygon;
			polygon.normal() = (*j)->constraint_plane().unit_normal();
			Polygon::PointInserter pi = polygon.point_inserter();
			*pi++ = new Point(*(*i) + 5*side);
			*pi++ = new Point(*(*i) + 5*side + 5*up);
			*pi++ = new Point(*(*i) - 5*side + 5*up);
			*pi   = new Point(*(*i) - 5*side);
			visualize(polygon);
			glEnable(*plane++);

			for(Polygon::PointIterator k = polygon.begin_points(); k != polygon.end_points(); ++k)
				delete(*k);
		}
		plane = planes;
		for(Feature::ConstCellIterator j = (*i)->begin_cells(); j != (*i)->end_cells(); ++j)
			glDisable(*plane++);
		
	}

	glPopMatrix();
}

}
}







