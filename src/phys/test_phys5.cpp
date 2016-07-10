#include "os/compat.h"
#include "main/types.h"
#include "main/3d_types.h"
#include <queue>
//#include <locale>
#include <GL/glut.h>
#include <time.h>
//#include <iostream.h>
#include <math.h>


#include "object/object.h"
#include "phys/phys.h"
#include "phys/pairs.h"
#include "world/triangle.h"
#include "phys/engine.h"
#include "world/world.h"

using namespace reaper;
using namespace reaper::object;


namespace reaper{

namespace phys{
	world::WorldRef dummy_world = reaper::world::World::create("test_level");
	reaper::phys::Engine theEngine;

	std::vector<DynamicBase*> objects;
        std::vector<StaticBase*> statics;
extern	std::vector<Triangle*> triangles;

	inline float timer() {return (float)clock()/(float)CLOCKS_PER_SEC;}

	double current_time = 0.0;	


//GL stuff
	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_position[] = {30.0, 30.0, 30.0, 0.0};
	GLfloat amb_light[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat red_mat[] = {1.0, 0.5, 0.5, 1.0};
	GLfloat blue_mat[] = {0.5, 0.5, 1.0, 1.0};


  void display(){
                
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glBegin(GL_TRIANGLES);
		for(int i = 0; i < triangles.size();++i)
		{
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(triangles[i]->a.x, triangles[i]->a.y, triangles[i]->a.z);
			glVertex3f(triangles[i]->b.x, triangles[i]->b.y, triangles[i]->b.z);
			glVertex3f(triangles[i]->c.x, triangles[i]->c.y, triangles[i]->c.z);
		}
		glEnd();
		
		
		reaper::world::dyn_iterator c, e;
		c = dummy_world->begin_dyn();
		e = dummy_world->end_dyn();
                int counter =0;

		for(; c!= e; ++c){
			glPushMatrix();

			glMultMatrixf(c->get_data().m);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
 
			glutWireSphere(10,10,10);        
			glPopMatrix();
                        ++counter;
                }

                
		reaper::world::st_iterator cs, es;
		cs = dummy_world->begin_st();
		es = dummy_world->end_st();

                for(; cs!= es; ++cs){
			glPushMatrix();

			glMultMatrixf(cs->get_data().m);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
 
			glutWireSphere(10, 10, 10);        
			glPopMatrix();
                }

		reaper::world::shot_iterator sh, sh_e;
                sh = dummy_world->begin_shot();
		sh_e = dummy_world->end_shot();
		
		for(; sh != sh_e; ++sh){
			glPushMatrix();

			glMultMatrixf(sh->get_data().m);
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
 
			glutWireSphere(10, 10, 10);        
			glPopMatrix();
		}


		glutSwapBuffers();
			

		if (current_time > 40.0)
			exit(0);

		float dt = timer() - current_time;        
		float temp_current=timer();

		double start = timer();	
		theEngine.update_world(temp_current,dt);
                cout << "Update: " << timer() - start<<endl;

		current_time = temp_current;
	}

	void gfx_init()
	{
		glEnable(GL_LIGHTING);

		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

		glLightfv(GL_LIGHT1, GL_AMBIENT, amb_light);

		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_NORMALIZE);
        
		//Set up camera
		glMatrixMode(GL_PROJECTION);
		gluPerspective( 40, 1.33, 0.01, 20000);
		glMatrixMode(GL_MODELVIEW);
		gluLookAt(10.0, 300.0, 300,
			  0.0, 30.0, -300.0,
			  0.0, 1.0, 0.0);
	}



}//Namespace
}
using namespace reaper::phys;

int main(int argc, char **argv)	
{

        reaper::phys::Triangle* tri;

/*        tri = new reaper::phys::Triangle(Vector( 0.0, 0.0, 50.0),
                Vector( 100.0, -100, 50),
                Vector( 0.0, 0, -100) );


        triangles.push_back(tri);

	tri = new reaper::phys::Triangle(Vector(0.0, 0, 50),
			Vector(0, 0, -100),
                        Vector( -100.0, -100, 0.0));
			
	triangles.push_back(tri);
*/

	DynamicBase* object;

        StaticBase* sobj;
        

        object = new Ship(SillyData(None,Matrix4(Point(-40,50,0))));
        dummy_world->add_object(object);

        object = new Ship(SillyData(None,Matrix4(Point(10,70,0))));
        dummy_world->add_object(object);

         object = new Ship(SillyData(None,Matrix4(Point(-20,50,-100))));
        dummy_world->add_object(object);

        object = new Ship(SillyData(None,Matrix4(Point(-10,65,50))));
        dummy_world->add_object(object);


        sobj = new Turret(SillyData(None,Matrix4(Point(-42,50,-300))));
        dummy_world->add_object(sobj);

        sobj = new Turret(SillyData(None,Matrix4(Point(-42,50,-600))));
        dummy_world->add_object(sobj);

        sobj = new Turret(SillyData(None,Matrix4(Point(-100,50,-800))));
        dummy_world->add_object(sobj);
        sobj = new Turret(SillyData(None,Matrix4(Point(-10,20,-400))));
        dummy_world->add_object(sobj);
        sobj = new Turret(SillyData(None,Matrix4(Point(10,45,-100))));
        dummy_world->add_object(sobj);

        glutInitWindowSize( 600, 450);        
        glutInit(&argc,argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH );

        glutCreateWindow("Physics Testlab 2");

	glutDisplayFunc(display);
	glutIdleFunc(display);

	gfx_init();
        theEngine.reinit();

	current_time= 0.0;
        glutMainLoop();

        return 0;
}
