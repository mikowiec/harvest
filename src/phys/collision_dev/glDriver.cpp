#include "os/compat.h"
#include "main/types_io.h"

#include <vector>
//#include <locale>
#include <GL/glut.h>
#include <time.h>
#include <iostream.h>
#include <math.h>

#include "coll_obj.h"
#include "coll_hash.h"

using std::vector;

////////////////////////////
//The global constants that are used throughout the program
const float kGrid_distance = 2;
const int kGrid_size = 5;
const float kFloor_size = 10;
const float kradius = 0.4;
const float kStartX = -10;
const float kStartY = 0;
const float kStartZ = -10;
//////////////////////////////////////////////////////


inline float timer() {return (float)clock()/(float)CLOCKS_PER_SEC;}

/////////////////////
//GLOBAL VARIABLES!!!!!

float current_time = 0.0;
bool simulating = false;
vector<coll_obj> objs;
grid the_grid(kStartX,kStartY,kStartZ,kGrid_distance);
coll_hash the_Table;
////////////////////////////

//GL stuff
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_position[] = {30.0, 30.0, 30.0, 0.0};
GLfloat amb_light[] = {0.2, 0.2, 0.2, 1.0};
GLfloat red_mat[] = {1.0, 0.5, 0.5, 1.0};
GLfloat blue_mat[] = {0.5, 0.5, 1.0, 1.0};
GLfloat blue_transp[] = {0.5, 0.5, 1.0, 0.4};

void add_objs()
{
        objs.push_back(coll_obj(kradius,-2,10,-2));
        objs.push_back(coll_obj(kradius,3,8,-1));
}

void draw_box(const Point& p1,const Point& p2)
{
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue_transp);
        glBegin(GL_QUADS);
        glVertex3f(p1.x,p1.y,p1.z);
        glVertex3f(p1.x,p1.y,p2.z);
        glVertex3f(p1.x,p2.y,p2.z);
        glVertex3f(p1.x,p2.y,p1.z);

        glVertex3f(p1.x,p1.y,p1.z);
        glVertex3f(p2.x,p1.y,p1.z);
        glVertex3f(p2.x,p2.y,p1.z);
        glVertex3f(p1.x,p2.y,p1.z);

        glVertex3f(p1.x,p1.y,p1.z);
        glVertex3f(p2.x,p1.y,p1.z);
        glVertex3f(p2.x,p1.y,p2.z);
        glVertex3f(p1.x,p1.y,p2.z);

        glVertex3f(p1.x,p2.y,p1.z);
        glVertex3f(p2.x,p2.y,p1.z);
        glVertex3f(p2.x,p2.y,p2.z);
        glVertex3f(p1.x,p2.y,p2.z);

        glVertex3f(p2.x,p1.y,p1.z);
        glVertex3f(p2.x,p1.y,p2.z);
        glVertex3f(p2.x,p2.y,p2.z);
        glVertex3f(p2.x,p2.y,p1.z);

        glVertex3f(p1.x,p1.y,p2.z);
        glVertex3f(p2.x,p1.y,p2.z);
        glVertex3f(p2.x,p2.y,p2.z);
        glVertex3f(p1.x,p2.y,p2.z);

        glEnd();
}

void draw_objects()
{
                
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
        for(int i=0;i < objs.size();++i){
                objs[i].render();
                
                vector<bucket_key> keys;

                Point pos = objs[i].get_pos();
                Point p1,p2;
                float rad = objs[i].get_radius();
                p1=pos;
                p2=pos;                
                p1.x-=rad;
                p1.y-=rad;
                p1.z-=rad;
                p2.x+=rad;
                p2.y+=rad;
                p2.z+=rad;

                
                the_grid.calc_key(p1,p2,keys);

                for(int i = 0; i< keys.size(); ++i){
                        Point p1t,p2t;
                        the_grid.get_points(keys[i],p1t,p2t);                        
                        draw_box(p1t,p2t);
                }
        }
}

void draw_floor(float size)
{
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue_mat);
        glBegin(GL_POLYGON);
                glVertex3f(-size,0,size);
                glVertex3f(size,0,size);
                glVertex3f(size,0,-size);
                glVertex3f(-size,0,-size);
        glEnd();
}

void draw_grid(int num, float distance)
{
        float start = distance*(num/2);
        
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
        glBegin(GL_LINES);
        for(int j = 0;j < num;++j)
                for(int i = 0;i < num;++i)
                {
                        glVertex3f(-start,j*distance,i*distance - start);
                        glVertex3f( start,j*distance,i*distance - start);
                }

        for(int j = 0;j < num;++j)
                for(int i = 0;i < num;++i)
                {
                        glVertex3f(j*distance - start,i*distance,start);
                        glVertex3f(j*distance - start,i*distance,-start);
                }

        for(int j = 0;j < num;++j)
                for(int i = 0;i < num;++i)
                {
                        glVertex3f(j*distance - start,2*start,i*distance-start);
                        glVertex3f(j*distance - start,0,      i*distance-start);
                }

        glEnd();
}

void simulate_objs(float dt)
{
        for(int i=0;i<objs.size();++i)
                objs[i].simulate(dt);
}       

void switch_simulate()
{
        if(simulating){
                simulating = false;
        }
        else{
                simulating = true;
                current_time = timer();
        }
}


void keyboard(unsigned char key,int x,int y)
{
        switch(key)
        {
        case 's': switch_simulate();
                break;
        default: //Do nothing
                break;

        }
}

void display(){
        float old_time = current_time;
        current_time = timer();
        float dt = current_time - old_time;
        if(simulating)
                simulate_objs(dt);
        

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);

        draw_grid(kGrid_size,kGrid_distance);
        draw_floor(kFloor_size);
        draw_objects();

        glutSwapBuffers();	
}

void gfx_init()
{
        glEnable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE,GL_SRC_ALPHA);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

        glLightfv(GL_LIGHT1, GL_AMBIENT, amb_light);

        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_NORMALIZE);
        
        //Set up camera
        glMatrixMode(GL_PROJECTION);
        gluPerspective( 50, 1.33, 0.01, 100);
        glMatrixMode(GL_MODELVIEW);
        gluLookAt(8.0, 3.0, 20,
                0.0, 0.0, 0.0,
                0.0, 1.0, 0.0);
}


int main(int argc, char **argv)	
{
        glutInitWindowSize( 768, 576);        
        glutInit(&argc,argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH );

        glutCreateWindow("Physics Testlab");
        glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutIdleFunc(display);

	gfx_init();

        add_objs();

        glutMainLoop();

        return 0;
}






