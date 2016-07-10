

#include "os/compat.h"

#include "main/types.h"
#include "gfx/gfx.h"
#include "gfx/managers.h"
#include "gfx/settings.h"
#include "gfx/camera.h"
#include "os/debug.h"
#include "os/event.h"
#include "os/gfx.h"
#include "os/gl.h"
#include "os/profile.h"
#include "os/time.h"
#include "misc/font.h"
#include "misc/parse.h"
#include "misc/sequence.h"
#include "object/ai.h"
#include "object/factory.h"
#include "object/forward.h"
#include "object/object.h"
#include "object/object_impl.h"
#include "object/phys.h"
#include "object/phys_data.h"
#include "res/config.h"
#include "res/res.h"
#include "world/world.h"

#include <iostream>
#include <vector>
#include <vector>


#include "misc/test_main.h"

using namespace reaper;
using namespace reaper::object;


class Stupid : public object::ObjImpl<object::DynamicBase>
{
	reaper::object::phys::ObjectAccessor phys;
public:
	Stupid(const Matrix& mtx)
	 : object::ObjImpl<object::DynamicBase>(object::MkInfo("shipX", mtx))
	 , phys(data, dyn_data, 1, 1, 1, 10)
	{
	}

	void collide(const object::CollisionInfo&) { }
	void gen_sound(reaper::sound::Interface&) { }
	gfx::RenderInfo* render(bool) const {
		Point p = get_pos();
		glVertex3f(p.x, p.y, p.z);
		return 0;
	}
	void simulate(double) { }
	void think() { }
	void receive(const reaper::ai::Message&) { }
	void add_waypoint(const Point&) { }
	void del_waypoint(Point&) { }
	const object::phys::ObjectAccessor& get_physics() const {
		return phys;
	}
	object::phys::ObjectAccessor& get_physics() {
		return phys;
	}

	void move(Point p) {
		data.set_mtx(Matrix(p));
	}

};

os::gfx::Gfx* gx;

void foo()
{
	os::event::EventProxy ep = os::event::EventSystem::get_ref(0);
	os::time::Profiler prof;
	world::Sphere find_sp(Point(0,0,0), 100);
	world::Cylinder find_cyl(Point2D(0, 0), 10);
	world::Line line(Point(0, 0, 0), Point(0, 500, 0));
	Point fru_pos(-400, 700, -400);
	Vector fru_dir(200, -300, 0);
	Vector fru_up(0, 1, 0);
	float fru_fov_w = 100;
	float fru_fov_h = 45;
	float rot_x = 20, rot_y = 40;
	float dist = 2000;

	GLUquadricObj* glu_sp = gluNewQuadric();
	GLUquadricObj* glu_cyl = gluNewQuadric();

	world::WorldRef wr = world::World::get_ref();

	int step_x = 1000;
	int step_y =  500;
	int step_z = 1000;

	for (int x = 0; x < 1000; x += step_x) {
		for (int z = 0; z < 1000; z += step_z) {
			for (int y = 0; y < 1000; y += step_y) {
				wr->add_object(DynamicPtr(new Stupid(Matrix(Point(x,y,z)))));
			}
		}
	}
	glCullFace(GL_FRONT);
	reaper::glEnable(GL_BLEND);
	reaper::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, gx->current_mode().width, gx->current_mode().height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 4.0/3.0, 10, 10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float z_add = 0, r_add1 = 0, r_add2 = 0;

	bool do_z_add = false, do_r_add = false;
	int obj = 0;
	world::dyn_iterator c, e = wr->end_dyn();
	while (true) {
		namespace id = os::event::id;

		if (ep.key(id::Escape))
			break;

		if (ep.key(id::Left))     rot_y += 1;
		if (ep.key(id::Right))    rot_y -= 1;
		if (ep.key(id::Up))       rot_x += 1;
		if (ep.key(id::Down))     rot_x -= 1;
		if (ep.key(id::Home))     dist  -= 50;
		if (ep.key(id::End))      dist  += 50;

		if (ep.key('1')) obj = 1;
		if (ep.key('2')) obj = 2;
		if (ep.key('3')) obj = 3;
		if (ep.key('4')) obj = 4;
		if (ep.key('5')) obj = 5;
		if (ep.key('6')) obj = 6;

		float dx = 0;
		float dy = 0;
		float dz = 0;
		float dw = 0;
		float ds = 0;
		float dt = 0;
		float du = 0;
		float dv = 0;
		if (ep.key('A')) dx = 10;
		if (ep.key('Z')) dx = -10;
		if (ep.key('S')) dy = 10;
		if (ep.key('X')) dy = -10;
		if (ep.key('D')) dz = 10;
		if (ep.key('C')) dz = -10;
		if (ep.key('F')) dw = 10;
		if (ep.key('V')) dw = -10;
		if (ep.key('G')) ds = 10;
		if (ep.key('B')) ds = -10;
		if (ep.key('H')) dt = 10;
		if (ep.key('N')) dt = -10;
		if (ep.key('J')) du = 10;
		if (ep.key('M')) du = -10;
		if (ep.key('K')) dv = 10;
		if (ep.key('L')) dv = -10;

		static bool skip_p = false;
		if (ep.key('P')) {
			if (!skip_p) {
				wr->update();
				int x = -1000, y = 0, z = -1000;
				for (c = wr->begin_dyn(); c != e; ++c) {
					c->get_physics().set_mtx(Matrix(Point(x,y,z)));
					if ((x += step_x) >= 1000) {
						x = -1000;
						if ((y += step_y) >= 1000) {
							y = 0;
							z += step_z;
						}
					}
				}
				skip_p = true;
			}
			
		} else {
			skip_p = false;
		}
		static bool skip_u = false;
		if (ep.key('U')) {
			if (!skip_u) {
				wr->update();
				skip_u = true;
			}
		} else {
			skip_u = false;
		}

		switch (obj) {
		case 1:
			find_sp.p.x += dx;
			find_sp.p.y += dy;
			find_sp.p.z += dz;
			find_sp.r += dw;
			break;
		case 2:
			find_cyl.p.x += dx;
			find_cyl.p.y += dy;
			find_cyl.r += dz/100;
			break;
		case 3:
			line.p1.x += dx;
			line.p1.y += dy;
			line.p1.z += dz;
			line.p2.x += dw;
			line.p2.y += ds;
			line.p2.z += dt;
			break;
		case 4:
			fru_pos.x += dx;
			fru_pos.y += dy;
			fru_pos.z += dz;
			fru_dir.x += dw;
			fru_dir.y += ds;
			fru_dir.z += dt;
			fru_fov_w += du;
			fru_fov_h += dv;
			fru_up = norm(cross(Vector(fru_dir.z, 0, fru_dir.x), fru_dir));
			break;
		default: ;
		}


		if (ep.button(0)) {
			z_add = ep.axis(1);
			do_z_add = true;
		} else {
			do_z_add = false;
		}
		if (ep.button(1)) {
			r_add1 = ep.axis(0);
			r_add2 = ep.axis(1);
			do_r_add = true;
		} else {
			do_r_add = false;
		}

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glTranslatef(0, 0, -dist);
		glRotatef(rot_x, 1, 0, 0);
		glRotatef(rot_y, 0, 1, 0);


		int m = 0;
		glPointSize(2);
		glColor3f(1,1,1);
		glBegin(GL_POINTS);
		for (c = wr->begin_dyn(); c != e; ++c, ++m) {
			c->render(false);
		}
		glEnd();

		reaper::glEnable(GL_DEPTH_TEST);


		glPointSize(5);

		int n = 0;

		switch (obj) {
		case 1:
			reaper::glDisable(GL_BLEND);
			glColor3f(1, 0, 0);
			glBegin(GL_POINTS);
			prof.start();
			for (c = wr->find_dyn(find_sp); c != e; ++c, ++n) {
				glVertex3f(c->get_pos().x, c->get_pos().y, c->get_pos().z);
			}
			prof.stop();
			glEnd();

			reaper::glEnable(GL_BLEND);
			glColor4f(0, 0, 0.5, 0.5);
			glPushMatrix();
			glTranslatef(find_sp.p.x, find_sp.p.y, find_sp.p.z);
			gluSphere(glu_sp, find_sp.r, 20, 20);
			glPopMatrix();

			break;
		case 2:
			reaper::glDisable(GL_BLEND);
			glColor3f(1, 0, 0);
			glBegin(GL_POINTS);
			prof.start();
			for (c = wr->find_dyn(find_cyl); c != e; ++c, ++n) {
				glVertex3f(c->get_pos().x, c->get_pos().y, c->get_pos().z);
			}
			prof.stop();
			glEnd();

			reaper::glEnable(GL_BLEND);
			glColor4f(0, 0, 0.5, 0.5);
			glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			glTranslatef(find_cyl.p.x, -find_cyl.p.y, 0);
			gluCylinder(glu_cyl, find_cyl.r, find_cyl.r, 10000, 20, 20);
			glPopMatrix();

			break;
		case 3:
			reaper::glDisable(GL_BLEND);
			glColor3f(1, 0, 0);
			glBegin(GL_POINTS);
			prof.start();
			for (c = wr->find_dyn(line); c != e; ++c, ++n) {
				glVertex3f(c->get_pos().x, c->get_pos().y, c->get_pos().z);
			}
			prof.stop();
			glEnd();

			reaper::glEnable(GL_BLEND);
			glColor3f(0,0,1);
			glBegin(GL_LINES);
			glVertex3fv(line.p1.get());
			glVertex3fv(line.p2.get());
			glEnd();
	
			break;
		case 4:
			reaper::glDisable(GL_BLEND);
			glColor3f(1, 0, 0);
			glBegin(GL_POINTS);
			prof.start();
			for (c = wr->find_dyn(world::Frustum(fru_pos, fru_dir, fru_up, fru_fov_w, fru_fov_h)); c != e; ++c, ++n) {
				glVertex3f(c->get_pos().x, c->get_pos().y, c->get_pos().z);
			}
			prof.stop();
			glEnd();

			reaper::glEnable(GL_BLEND);
			Vector left = norm(cross(fru_up, fru_dir));
			float len = length(fru_dir);
			float w = tan(3.14/180*fru_fov_w/2) * len;
			float h = tan(3.14/180*fru_fov_h/2) * len;
			Point ul = fru_pos + fru_dir + fru_up*h + left*w;
			Point ll = fru_pos + fru_dir - fru_up*h + left*w;
			Point ur = fru_pos + fru_dir + fru_up*h - left*w;
			Point lr = fru_pos + fru_dir - fru_up*h - left*w;

			glBegin(GL_TRIANGLES);
			glColor4f(0, 0, 0.5, 0.5);
			glVertex3fv(fru_pos.get());
			glVertex3fv(lr.get());
			glVertex3fv(ur.get());
			glVertex3fv(fru_pos.get());
			glVertex3fv(ur.get());
			glVertex3fv(ul.get());
			glVertex3fv(fru_pos.get());
			glVertex3fv(ul.get());
			glVertex3fv(ll.get());
			glVertex3fv(fru_pos.get());
			glVertex3fv(ll.get());
			glVertex3fv(lr.get());
			
			glVertex3fv(ur.get());
			glVertex3fv(ll.get());
			glVertex3fv(ul.get());

			glVertex3fv(ll.get());
			glVertex3fv(ur.get());
			glVertex3fv(lr.get());
			glEnd();

			break;
		}

		derr << "objs: " << m << " find: " << n << "  " << prof.report();

		gx->update_screen();
		if (exit_now())
			break;
	}
}


int test_main()
{
	gx = new os::gfx::Gfx();

	os::event::EventSystem es(*gx);
	world::WorldRef wr = world::World::create();
	wr->load("test_level");
	gfx::TextureRef::create();
	gfx::MeshRef::create();

	foo();

	return 0;
}

