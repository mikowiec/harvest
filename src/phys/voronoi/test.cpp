#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include <time.h>

#include "main/types.h"
#include "main/types.h"

#include "vertex.h"
#include "distance.h"
#include "visualize.h"
#include "polyhedron.h"
#include "mesh.h"

GLfloat red_mat[]  = {1.0, 0.5, 0.5, 1.0};
GLfloat blue_mat[] = {0.5, 0.5, 1.0, 1.0};

reaper::phys::Polyhedron *box;
reaper::phys::Polyhedron *box2;
reaper::phys::ClosestFeatures *cf;


void visualize()
{
	static int i = 0;
	i++;
	GLfloat specular[] = {0.2, 0.2, 0.2, 1};
	GLfloat shininess[] = { 50.0 };
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	glPushMatrix();

	box->matrix_mult(reaper::Matrix4(0.3, reaper::Vector(0.25, 1, 0.1)));
//	box->matrix_mult(reaper::Matrix4(reaper::Vector(0.1, 0.1, 0)));

	box2->matrix_mult(reaper::Matrix4(0.2, reaper::Vector(0.8, 0.5, 0.4)));
	
	delete(cf);

	cf = close_feat_checkp(*(*box->begin_vertices()), *(*box2->begin_vertices()));

	std::cout << "distance: " << cf->distance << std::endl;

//	glRotatef((float)clock()/(float)CLOCKS_PER_SEC*25, 0, 1, 0);
//	glRotatef((float)clock()/(float)CLOCKS_PER_SEC*10, 1, 0, 0);
//	glutWireSphere(1, 10, 10);
	reaper::phys::visualize(*box);
	reaper::phys::visualize(*box2);

	glPopMatrix();

	glutSwapBuffers();
}

void gl_init()
{
//	glShadeModel(GL_SMOOTH);
	GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light0_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light0_position[] = {0.5, 1.0, 5.0, 1.0};
	GLfloat light0_direction[] = {0.0, 0.0, -1.0, 1.0};

	glClearColor(1.0, 1.0, 1.0, 0.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_direction);
	
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	glMatrixMode(GL_PROJECTION);
	gluPerspective(40, 1.33, 0.01, 10000);

	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0, 0, 25, 0, 0, 0, 0, 1, 0);

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glEnable(GL_CULL_FACE);
	
}

int main(int argc, char **argv)
{
	std::cout << "Building box..\n";
	box = reaper::phys::cube(1, 1, 1);
	box2 = reaper::phys::cube(0.5, 1, 1.5);

	box->matrix_mult(reaper::Matrix4(reaper::Vector4(-5.0, 0.0, 0.0)));
	box2->matrix_mult(reaper::Matrix4(reaper::Vector4(6.0, 3.0, 0.0)));

	cf = close_feat_checkp(*(*box->begin_vertices()), *(*box2->begin_vertices()));
	std::cout << "Done..\n";

	std::cout << "Clocks per sec: " << CLOCKS_PER_SEC << "\n";

	glutInitWindowSize(600, 480);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	
	glutCreateWindow("Polyhedron Visualization");

	glutDisplayFunc(visualize);
	glutIdleFunc(visualize);

	gl_init();

	int max;
	glGetIntegerv(GL_MAX_CLIP_PLANES, &max);
	std::cout << "max planes: " << max << std::endl;

	glutMainLoop();
	
	return 0;
}



