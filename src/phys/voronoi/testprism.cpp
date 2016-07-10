#include "GL/glut.h"
#include "os/compat.h"

#include "main/types.h"
#include "main/types.h"

#include <iostream>
#include <cmath>
#include <limits>
#include <typeinfo>

#include "polyhedron.h"
#include "face.h"
#include "vertex.h"
#include "edge.h"
#include "plane.h"
#include "cell.h"
#include "winged.h"

#include "distance.h"
#include "geometry.h"

using namespace reaper::phys;
using namespace std;

double angle;

Face *face;
Edge *edge;
Edge *edge2;

void display_edge(const Edge &edge)
{
	cout << "disp. edge" << endl; 
	cout << "(" << edge.tail().x << ", " << edge.tail().y << ", " << edge.tail().z << ")->" <<
		"(" << edge.head().x << ", " << edge.head().y << ", " << edge.head().z << ")" << endl;
	glBegin(GL_LINES);
	glVertex3fv(edge.tail());
	glVertex3fv(edge.head());
	glEnd();
}

void build_cells(Face *face)
{
	Face::VertexIterator prev = --face->end_vertices();
	for(Face::VertexIterator curr = face->begin_vertices(); curr != face->end_vertices(); ++curr){
		*face->cell_inserter() = new Cell(Plane(cross_product(face->normal(), *(*curr)-*(*prev)), *(*curr)), 
						  new Vertex()); // nope, that vertex is no good
		prev = curr;
	}
}



void display()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	// draw closest
	
	const Feature *closest = closest_to_E(*edge, *face);
	glColor3f(1.0, 1.0, 1.0);
	if(typeid(*closest)==typeid(Winged)){
		cout << "closest is winged" << endl;
		const Winged *close = (Winged*)closest;
		display_edge(Edge(new Point(close->tail() + reaper::Point(0.0, 0.0, 0.6)), new Point(close->head() + reaper::Point(0.0, 0.0, 0.6))));
	}
	else if(typeid(*closest)==typeid(Vertex)){ 
		cout << "closest is vertex" << endl;
		glBegin(GL_POINTS);
		glVertex3fv(*(Vertex*)closest + reaper::Vector(0.0, 0.0, 0.2));
		glEnd();
	}
	else
		cout << "truly very strange" << endl;

	// draw polygon
	glColor3f(1.0, 0.0, 0.0);
	
	glBegin(GL_POLYGON);
  	for(Face::VertexIterator i = face->begin_vertices(); i != face->end_vertices(); ++i){
		cout << "(" << (*i)->x << ", " << (*i)->y << ", " << (*i)->z << ")" << endl;
		glVertex3fv(*(*i));
	}
  	glEnd();

	glColor3f(0.0, 0.0, 1.0);
	display_edge(*edge);
	display_edge(*edge2);

	// draw nearest points

	Point near1, near2;
	
	nearest_pts(*edge, near1, *edge2, near2);
	
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POINTS);
	glVertex3fv(near1 + reaper::Vector(0.0, 0.0, 0.2));
	glColor3f(0.5, 0.5, 0.5);
	glVertex3fv(near2 + reaper::Vector(0.0, 0.0, 0.2));
	glBegin(GL_POINTS);
	glEnd();

	// draw point

	Point nearest = nearest_pt(edge->head(), *face);

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_POINTS);
	glVertex3fv(nearest + reaper::Vector(0.0, 0.0, 0.2));
	glEnd();

	nearest = nearest_pt(Point(0.5, 0.5, 0.0), *edge);

	glBegin(GL_POINTS);
	glVertex3fv(nearest + reaper::Vector(0.0, 0.0, 0.2));
	glEnd();

	glPopMatrix();
	glutSwapBuffers();
}

void readkeys(unsigned char key, int x, int y)
{
	switch(key){
	case 'o':
		edge->tail() = edge->tail() + 
			reaper::Matrix4(angle, reaper::Vector(0.0, 0.0, 1.0))*Point(0.0, 0.05, 0.0);
		break;
	case 'l':
		edge->tail() = edge->tail() + 
			reaper::Matrix4(angle, reaper::Vector(0.0, 0.0, 1.0))*Point(0.0, -0.05, 0.0);
		break;
	       
	case 'k':
		angle += 0.4;
		*edge = reaper::Matrix4(0.4, reaper::Vector(0.0, 0.0, 1.0))*(*edge);
		break;
		
	case ';':
		angle -= 0.4;
		*edge = reaper::Matrix4(-0.4, reaper::Vector(0.0, 0.0, 1.0))*(*edge);
		break;
		
	}
	cout << "above prism: " << E_FPrism(*edge, *face) << endl;

	display();
}

void main(int argc, char **argv)
{
	face = new Face();

	Face::VertexInserter i = face->vertex_inserter();

	face->normal() = reaper::Vector(0.0, 0.0, 1.0);

	Vertex *vertex = new Vertex(Point(-0.5,  0.5, 0.0));

	*i++ = new Vertex(Point(-0.5,  0.5, 0.0));
	*i++ = new Vertex(Point(-0.5, -0.5, 0.0));
	*i++ = new Vertex(Point( 0.5, -0.5, 0.0));
	*i   = new Vertex(Point( 0.5,  0.5, 0.0));

	Face::EdgeInserter j = face->winged_inserter();

	*j++ = new Winged(*face->begin_vertices(),     *(face->begin_vertices()+1), new Face(), face);
	*j++ = new Winged(*(face->begin_vertices()+2), *(face->begin_vertices()+1), face, new Face());
	*j++ = new Winged(*(face->begin_vertices()+2), *(face->begin_vertices()+3), new Face(), face);
	*j   = new Winged(*face->begin_vertices(),     *(face->begin_vertices()+3), face, new Face());

	build_cells(face);

	angle = 0.0;

	edge = new Edge(new Point(0.75, -0.5, 0.1), new Point(0.75, 0.3, 0.1));
	edge2 = new Edge(new Point(-0.75, -0.3, 0.1), new Point(-0.6, 0.3, 0.1));

	// Init graphics

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Edge in prism test");

	glutDisplayFunc(display);	
//	glutIdleFunc(display);
	glutKeyboardFunc(readkeys);

	// Init graphics
	
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0, 1.0, -1.0, 1.0, 0.0, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0,
		  0.0, 0.0, 0.0,
		  0.0, 1.0, 0.0);

	glutMainLoop();
	
	return 0;
}






