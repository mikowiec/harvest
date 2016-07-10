#include "os/compat.h"
#include "main/types_io.h"

#include <GL/glut.h>
#include <time.h>

#include "res/config.h"
#include "world/worldref.h"
#include "world/world.h"
#include "world/triangle.h"
#include "phys/engine.h"
#include "object/base.h"
#include "object/object_impl.h"
#include "object/phys.h"


using namespace reaper;
using namespace reaper::world;
using namespace reaper::object;

reaper::world::WorldRef wr;
reaper::phys::PhysRef pr;

namespace reaper {

namespace gfx {

	class RenderInfo;

}

}
/////////////////////
//GLOBAL VARIABLES!!!!!

float current_time = 0.0;
bool simulating = false;
////////////////////////////


//GL stuff
GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
GLfloat light_position[] = {30.0, 30.0, 30.0, 0.0};
GLfloat amb_light[] = {0.2, 0.2, 0.2, 1.0};
GLfloat red_mat[] = {1.0, 0.5, 0.5, 1.0};
GLfloat blue_mat[] = {0.5, 0.5, 1.0, 0.0};
GLfloat green_mat[] = {0.5, 1.0, 0.5, 1.0};


class testSilly
 : public SillyBase
{
	SillyData sdata;
public:


	testSilly(const Matrix& mtx, float rad)

		: SillyBase(sdata), sdata(mtx, Nature, "foo", rad)

	{ }

	virtual const gfx::RenderInfo* render(bool effects) const

	{

		return 0;

	}

	virtual void collide(const CollisionInfo&) 

	{

	}
}; 

class testDyn
 : public DynamicBase
{
	SillyData sdata;
	DynamicData ddata;
	ObjectAccessor acc;
public:


	testDyn(const Matrix& mtx, float rad)

		: DynamicBase(sdata), sdata(mtx, Nature, "foo", rad), 
		 ddata(), acc(sdata,ddata, 100, 1, 1, rad)

	{ }

	virtual const gfx::RenderInfo* render(bool effects) const

	{

		return 0;

	}

	virtual void collide(const CollisionInfo&) 

	{

	}
	
	virtual void think() {} 
	virtual void simulate(double dt);
	virtual void receive(const ::reaper::ai::Message &) {} ;

	virtual void add_waypoint(const Point& p){} ;
	virtual void del_waypoint(Point& p) {} ;
	virtual       reaper::object::phys::ObjectAccessor& get_physics() { return acc;};
	virtual const reaper::object::phys::ObjectAccessor& get_physics() const { return acc;};

};

void testDyn::simulate(double dt)
{
	//S simple model with gravitation
	Matrix m = acc.get_mtx();
	Point pos = m.pos() ;
	pos +=  dt*ddata.velocity;
	m.pos() = pos;
	acc.set_mtx(m);
	ddata.velocity+=dt*Vector(0,-10,0);
	sim_time+=dt;
}
 

void render(DynamicPtr ptr)
{
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue_mat);
	Point pos = ptr->get_pos();
	float radius = ptr->get_radius();
	glTranslatef(pos.x,pos.y,pos.z);
        glutSolidSphere(radius,15,15);
	glPopMatrix();
}

void render(SillyPtr ptr)
{

	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
	Point pos = ptr->get_pos();
	float radius = ptr->get_radius();
	glTranslatef(pos.x,pos.y,pos.z);
        glutSolidSphere(radius,15,15);
	glPopMatrix();
}

void render(StaticPtr ptr)
{	
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green_mat);
	Point pos = ptr->get_pos();
	float radius = ptr->get_radius();
	glTranslatef(pos.x,pos.y,pos.z);
        glutSolidSphere(radius,8,8);
	glPopMatrix();
}

void render(ShotPtr ptr)
{
}

void render(Triangle* tri)
{
	Point p1(tri->a),p2(tri->b),p3(tri->c);


	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red_mat);
	glBegin(GL_TRIANGLES);
		glVertex3f(p1.x,p1.y,p1.z);
		glVertex3f(p2.x,p2.y,p2.z);
		glVertex3f(p3.x,p3.y,p3.z);
	glEnd();
}


inline float timer() {return (float)clock()/(float)CLOCKS_PER_SEC;}


void gfx_init()
{
        glEnable(GL_LIGHTING);
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_ONE,GL_SRC_ALPHA);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

        glLightfv(GL_LIGHT1, GL_AMBIENT, amb_light);

        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_NORMALIZE);
        
        //Set up camera
        glMatrixMode(GL_PROJECTION);
        gluPerspective( 50, 1.33, 0.1, 1000);
        glMatrixMode(GL_MODELVIEW);
        gluLookAt(120.0, 30.0, 60,
                0.0, 0.0, 0.0,
                0.0, 1.0, 0.0);
}

void display()
{
	dyn_iterator dyn = wr->begin_dyn();
	si_iterator si = wr->begin_si();
	st_iterator st = wr->begin_st();
	shot_iterator sh = wr->begin_shot();
	tri_iterator tri = wr->begin_tri();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	while(dyn != wr->end_dyn() ){
		render(*dyn);
		++dyn;
	}
	
	while(si != wr->end_si() ){
		render(*si);
		++si;
	}
	
	while(st != wr->end_st() ){
		render(*st);
		++st;
	}
	
	while(sh != wr->end_shot() ){
		render(*sh);
		++sh;
	}

	while(tri != wr->end_tri() ){
		render( *tri );
		++tri;
	}


	glutSwapBuffers();

	float old_time = current_time;
        current_time = timer();
        float dt = current_time - old_time;
//        if(simulating)
	while(dt > 0.1){
		pr->update_world(old_time,0.1);
		old_time+=0.1;
		dt-=0.1;
	}

	pr->update_world(old_time,dt);

	cout << "Update time " << dt << endl;

}

void add_objs()
{
	for (int i = 0; i < 10; ++i)
		for(int j = 0; j < 10; ++j){
			SillyPtr ptr = new testSilly( Matrix(Vector(i*5 - 20,0,j*5 - 20)), 0.95 );
			wr->add_object(ptr);
		}

	wr->add_object(DynamicPtr(new testDyn( Matrix(Vector(5.5,10,0.05)), 2.0 )));
	

	for (int i = 0; i < 3; ++i)
		for(int j = 0; j < 3; ++j){
		wr->add_object(DynamicPtr(new testDyn( Matrix(Vector(i*8 - 10,100, j*7 -10 )), 1.4 )));
	}
	Triangle* newtri = new Triangle( *(new Point(-50.0,0.0,50.0)) ,
					*(new Point(50.0,0.0,-50.0)) ,
					*(new Point(-50.0,0.0,-50.0)) );

	wr->add_object(newtri);

	newtri = new Triangle( *(new Point(50.0,0.0,-50.0)) ,
				*(new Point(-50.0,0.0,50.0)),
				*(new Point(50.0,0.0,50.0)) );
	wr->add_object(newtri);
}

int main(int argc, char **argv)	
{
	
	wr = reaper::world::World::create();
	wr->load("empty_level");

	add_objs();
	
	pr = reaper::phys::Engine::create();

	
	glutInitWindowSize( 768, 576);        
        glutInit(&argc,argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB |GLUT_DEPTH );

        glutCreateWindow("Physics Testlab");
//        glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutIdleFunc(display);

	gfx_init();

	current_time = timer();
        glutMainLoop();

        return 0;
}






