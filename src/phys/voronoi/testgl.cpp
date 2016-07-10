#include <time.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

GLfloat red_mat[]  = {1.0, 0.5, 0.5, 1.0};
GLfloat blue_mat[] = {0.5, 0.5, 1.0, 1.0};


void visualize()
{
	GLfloat specular[] = {0.2, 0.2, 0.2, 1};
	GLfloat shininess[] = { 50.0 };
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	glPushMatrix();
	
	glRotatef(clock()/(float)CLOCKS_PER_SEC*20, 0, 1, 0);
	glRotatef(clock()/(float)CLOCKS_PER_SEC*10, 1, 0, 0);
	glutWireSphere(1, 10, 10);

	glPopMatrix();

	glutSwapBuffers();
}


void gl_init()
{
	glShadeModel(GL_SMOOTH);
	GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light0_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light0_position[] = {0.5, 1.0, 5.0, 1.0};
	GLfloat light0_direction[] = {0.0, 0.0, -1.0, 1.0};


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
	gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
	
}

int main(int argc, char **argv)
{
	glutInitWindowSize(600, 480);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	
	glutCreateWindow("Polyhedron Visualization");

	glutDisplayFunc(visualize);
	glutIdleFunc(visualize);

	gl_init();
	glutMainLoop();
	
	return 0;
}




























