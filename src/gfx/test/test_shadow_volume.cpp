#include "os/compat.h"

#include "main/types.h"
#include "main/types_io.h"

#include <vector>

#include "gfx/abstracts.h"
#include "gfx/gfx.h"
#include "gfx/managers.h"
#include "gfx/exceptions.h"
#include "gfx/settings.h"
#include "gfx/camera.h"
#include "gfx/misc.h"
#include "gfx/displaylist.h"
#include "gfx/light.h"
#include "gfx/fps_meter.h"

#include "os/gl/gl_info.h"
#include "os/debug.h"
#include "os/gfx.h"
#include "os/gl.h"
#include "msg/event.h"
#include "os/time.h"

#include "gfx/geometry.h"

namespace reaper {
debug::DebugOutput dout("TestShadowVolume");

using namespace std;
using namespace gfx;

ostream& operator << (ostream& os, const mesh::Edge &e)
{
	os << "p1: " << e.p1 << " p2: " << e.p2;
	os << " t1: " << e.t1 << " t2: " << e.t2;
	return os;
}

ostream& operator << (ostream& os, const mesh::Triangle &e)
{
	os << PtrTriangle(e) << " n: " << e.n << " lit: " << e.lit;
	return os;
}

template<class T>
ostream& operator << (ostream &os, const vector<T> &v)
{
	for(std::vector<T>::const_iterator i = v.begin(); i != v.end(); ++i ) {
		os << *i << "\n";
	}
	return os;
}

int foo()
{
	// init gl system

	os::gfx::Gfx gx;
	os::event::EventSystem es(gx);
	os::event::EventProxy ep = os::event::EventSystem::get_ref(os::event::System);

	gfx::TextureRef::create();
	gfx::MeshRef::create();
	gfx::TextureRef tr = gfx::TextureMgr::get_ref();
	gfx::MeshRef mr = gfx::MeshMgr::get_ref();

	gfx::Settings &s = gfx::Settings::current;
        gfx::init_gl_settings(s);
	if(s.stencil_bits == 0) {
		dout << "need stencil buffer!\n";
		return 1;
	}

	throw_on_gl_error("system init");

	// initialize geometry

	Matrix mtx = Matrix(Point(0,6,0)) * Matrix(45, norm(Vector(1,1,0)));
	RenderInfo ri("cube", mtx, false);

	Vector light_dir = norm(Vector(1,1,0));
	light::Light light(light_dir, Color(1,1,1));
	light.activate(0);

		mesh::ShadowVolume sv;
	mr->generate_shadow_volume(ri.mesh, light_dir, mtx, sv);
			
	/*
	dout << "sv tris: " << sv.triangles.size() << "\n";
	dout << "sv points: " << sv.points.size() << "\n";
	dout << "gm points: " << sv.geometry->points.size() << "\n";
	dout << "gm tris: " << sv.geometry->triangles.size() << "\n";
	dout << "gm edges: " << sv.geometry->edges.size() << "\n\n";
	*/

	// intialize gl

	glClearColor(0,0,0,0);
	glColor3f(.4,.4,.4 );
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);

	gfx::misc::DisplayList list;
	gfx::Material m1, m2;
	m1.diffuse_color = Color(1,1,1);
	m1.specular_color = Color(0,0,0);
	m1.ambient_color = Color(1,0,0);
	m1.emission_color = Color(0,0,0);
	m1.shininess = 0;

	m2.diffuse_color = Color(1,0,1);
	m2.specular_color = Color(1,1,1);
	m2.ambient_color = Color(1,1,1);
	m2.emission_color = Color(0,0,0);
	m2.shininess = 20;

	gfx::misc::DisplayList sphere;
	sphere.begin();
	gfx::misc::sphere(.3,10,10);
	sphere.end();

	list.begin();
		
		m1.use();
		glBegin(GL_QUADS);
		glNormal3f(0,1,0);
		glVertex3f(10,1.5,10);
		glVertex3f(10,1.5,-10);
		glVertex3f(-10,1.5,-10);
		glVertex3f(-10,1.5,10);
		glEnd();
		
		m2.use();
		for(int x = -10; x < 11; x++)
			for(int y = -10; y < 11; y++) {
				glMaterialfv(GL_FRONT, GL_DIFFUSE, Color((x+10.0) / 20.0, -((y+10.0) / 20.0), (y+10.0) / 20.0).get());
				glPushMatrix();
				glTranslatef(x,3,y);
				sphere.call();
				glPopMatrix();
			}
	list.end();
	
	FPSMeterVP fps;

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	throw_on_gl_error("gl_init");

	dout << "Starting...\n";

	float zero[] = {0,0,0,0};
	float ambient[] = {.2,.2,.2,0};
		
	// main loop

	bool exit = false;
	int rendermode = 1;
	while(!exit) {
		os::event::Event e;
		while (ep.get_event(e)) {
			bool dn = e.is_pressed();
			switch (e.id) {
			case os::event::id::Escape: 
				exit = true; 
				break;
			case '0':
			case '1':
			case '2':
			case '3':
				rendermode = e.id - '0';
				dout << "rendermode: " << rendermode << "\n";
				break;
			}
		}	 

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		static float ang = 0;
		ang += .3;
		mtx = Matrix(Point(0,6,0));
		mtx = mtx * Matrix(ang, norm(Vector(1,.5,.5)));
		light_dir = norm(Vector(-sin(ang/30.0),1,-cos(ang/30.0)));
		light.pos.x = light_dir.x;
		light.pos.y = light_dir.y;
		light.pos.z = light_dir.z;
		light.activate(0);

		Camera cam(Point(sin(ang/50.0)*10,13 ,cos(ang/50.0)*10), Point(0,3,0), Vector(0,1,0), 100, 75);
		cam.setup_view();

		sv.reset();
		mr->generate_shadow_volume(ri.mesh, light_dir, mtx, sv);

		switch(rendermode) {
		case 0:
		case 1:
			glDisable(GL_BLEND);
			glEnable(GL_LIGHTING);			
			glEnable(GL_LIGHT0);
			tr->use("");
			list.call();

			glPushMatrix();
			glMultMatrix(mtx);
			mr->render(ri, 0);
			glPopMatrix();
			tr->use("");
			
			if(rendermode == 0)
				break;
			
			glDisable(GL_LIGHTING);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			glColor3f(.5,.5,.5);
			sv.render();
			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			break;
		case 2:
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glDepthMask(1);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			glEnable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

			// draw scene
			 tr->use("");
			 list.call();

			 glPushMatrix();
			 glMultMatrix(mtx);
			 mr->render(ri, 0);
			 glPopMatrix();
			 tr->use("");
			
			glDepthMask(0);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zero);

			// per lightsource (only one currently)
 			glClear(GL_STENCIL_BUFFER_BIT);
			glColorMask(0,0,0,0);
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_ALWAYS,0,~0);
			glStencilMask(~0);			

			// for all occluders
			 glCullFace(GL_FRONT);
			 glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
			 sv.render();

			 glCullFace(GL_BACK);
			 glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
			 sv.render();
			// end for all occluders

			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			glStencilFunc(GL_EQUAL, 0, ~0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glDepthFunc(GL_EQUAL);
			glColorMask(1,1,1,1);

			// draw scene
			 tr->use("");
			 list.call();

			 glPushMatrix();
			 glMultMatrix(mtx);
			 mr->render(ri, 0);
			 glPopMatrix();
			 tr->use("");
			
			// end per light source

			glDepthFunc(GL_LESS);
			glDisable(GL_BLEND);
			glDisable(GL_STENCIL_TEST);
			glDepthMask(1);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

			break;			
		case 3:
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);

			// draw scene
			 tr->use("");
			 list.call();

			 glPushMatrix();
			 glMultMatrix(mtx);
			 mr->render(ri, 0);
			 glPopMatrix();
			 tr->use("");
			
			glDepthMask(0);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);

			// per lightsource (only one currently)
 			glClear(GL_STENCIL_BUFFER_BIT);
			glColorMask(0,0,0,0);
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_ALWAYS,0,~0);
			glStencilMask(~0);			

			// for all occluders
			 glCullFace(GL_FRONT);
			 glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
			 sv.render();

			 glCullFace(GL_BACK);
			 glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
			 sv.render();
			// end for all occluders

			glStencilFunc(GL_NOTEQUAL, 0, ~0);
			glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
			glDisable(GL_DEPTH_TEST);
			glColorMask(1,1,1,1);
			glDisable(GL_BLEND);

			// draw big screen aligned poly
			glDisable(GL_LIGHTING);
			glColor3f(0,0,0);
			glMatrixMode(GL_PROJECTION);
			glOrtho(0,1,0,1,0,1);
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glLoadIdentity();
			glBegin(GL_QUADS);
			 glVertex2f(0,0);
			 glVertex2f(1,0);
			 glVertex2f(1,1);
			 glVertex2f(0,1);
			glEnd();
			glPopMatrix();

			// end per light source

			glDepthFunc(GL_LESS);
			glDisable(GL_BLEND);
			glDisable(GL_STENCIL_TEST);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(1);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

			break;
		}
		
		//fps.render();

		gx.update_screen();
	};	

	return 0;
}
}

int main()
{
	using namespace reaper;

	try {
		return foo();
	}
	catch(fatal_error_base &e) {
		dout << e.what() << "\n";
		return 1;
	}
	catch(error_base &e) {
		dout << e.what() << "\n";
		return 1;
	}
	catch(...) {
		throw;
	}
}
