#include <glut.h>
#include "os/compat.h"
#include "gfx/object.h"
#include "gfx/texture.h"
#include <iostream>
#include <windows.h>
#include <winuser.h>

using namespace reaper;
using namespace reaper::gfx;
using namespace reaper::gfx::internal;
using namespace std;

Matrix4 m(true);
float zoom = 1;

const char *name, *tex;
bool reload = false;

void myUpdate()
{
        try {
                static Object obj(name);
                static Texture *texture = new Texture(tex);
                if(reload) {
                        delete texture;
                        texture = new Texture(tex);
                        reload = false;
                }

                texture->use();
	        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
                glPushMatrix();
                glMultMatrixf(m);
                glScalef(zoom,zoom,zoom);
                obj.render(0);
                glPopMatrix();
	        glutSwapBuffers();
        }
        catch(fatal_error_base &e) {
                cout << e.what() << endl;
                exit(0);
        }
        catch(error_base &e) {
                cout << e.what() << endl;
                exit(0);
        }
}

void myIdle()
{
        bool change = false;
        if(GetAsyncKeyState(VK_LEFT))  { m = m*Matrix4(1,Vector(0,1,0)) ; change = true; }
        if(GetAsyncKeyState(VK_RIGHT)) { m = m*Matrix4(-1,Vector(0,1,0)) ; change = true; }
        if(GetAsyncKeyState(VK_UP))    { m = m*Matrix4(1,Vector(1,0,0)) ; change = true; }
        if(GetAsyncKeyState(VK_DOWN))  { m = m*Matrix4(-1,Vector(1,0,0)) ; change = true; }
        if(GetAsyncKeyState(VK_INSERT)) { m = m*Matrix4(1,Vector(0,0,1)) ; change = true; }
        if(GetAsyncKeyState(VK_DELETE)) { m = m*Matrix4(-1,Vector(0,0,1)) ; change = true; }
        if(GetAsyncKeyState(VK_PRIOR)) { zoom *= 1.02; change = true; }
        if(GetAsyncKeyState(VK_NEXT))  { zoom *= 0.98; change = true; }
        if(GetAsyncKeyState(VK_HOME))  { glEnable(GL_LIGHTING); change = true; }
        if(GetAsyncKeyState(VK_END))   { glDisable(GL_LIGHTING); change = true; }
        if(GetAsyncKeyState(VK_F1))   { glClearColor(0,0,0,0); change = true; }
        if(GetAsyncKeyState(VK_F2))   { glClearColor(.1,.1,.1,0); change = true; }       
        if(GetAsyncKeyState(VK_F3))   { glClearColor(.2,.2,.2,0); change = true; }       
        if(GetAsyncKeyState(VK_F4))   { glClearColor(.3,.3,.3,0); change = true; }       
        if(GetAsyncKeyState(VK_F5))   { glClearColor(.4,.4,.4,0); change = true; }       
        if(GetAsyncKeyState(VK_F6))   { glClearColor(.5,.5,.5,0); change = true; }       
        if(GetAsyncKeyState(VK_F7))   { glClearColor(.6,.6,.6,0); change = true; }       
        if(GetAsyncKeyState(VK_F8))   { glClearColor(.7,.7,.7,0); change = true; }       
        if(GetAsyncKeyState(VK_F9))   { glClearColor(.8,.8,.8,0); change = true; }       
        if(GetAsyncKeyState(VK_F10))   { glClearColor(.9,.9,.9,0); change = true; }       
        if(GetAsyncKeyState(VK_F11))   { glClearColor(1,1,1,0); change = true; }       
        if(GetAsyncKeyState(VK_F12))   { reload = true; change = true; }       
        if(GetAsyncKeyState(VK_ESCAPE))   { exit(0); }

        if(change) {
                if(zoom<0.1) zoom = 0.1;
                myUpdate();
        } else
                Sleep(5);
}


void myReshape(int w, int h)
{
	glViewport(0,0,w,h);    	// hela fönstret
	glMatrixMode(GL_PROJECTION); 
        glLoadIdentity();	
	gluPerspective(70,1,1,1000);
                        
	glMatrixMode(GL_MODELVIEW); 
        glLoadIdentity();
        gluLookAt(0,0,-20,0,0,0,0,1,0);
}

int main(int argc, char *argv[])
{	
        if(argc == 3) {
                name = argv[1];
                tex = argv[2];
        }
        else {
                cout << "Usage: obj_viewer object texture\n\n";
                cout << "Specify object and texture files without extensions!\n";
                return 0;
        }

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(200,100);
	glutCreateWindow("Reaper Object Viewer");

	glutReshapeFunc(myReshape);
	glutDisplayFunc(myUpdate);
        glutIdleFunc(myIdle);

        glEnable(GL_NORMALIZE);
        glEnable(GL_TEXTURE_2D);         
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHT0);

	glClearColor(0,0,0,0);
	glutMainLoop();

        return 0;
}
