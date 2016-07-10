
#include "os/compat.h"

#include "os/gfx.h"
#include "os/event.h"
#include "os/time.h"
#include "os/profile.h"
#include "os/debug.h"
#include "misc/font.h"
#include <vector>
#include "main/types.h"
#include "gfx/gfx.h"
#include "gfx/camera.h"
#include "gfx/settings.h"
#include "world/world.h"
#include "os/gl.h"
#include "res/res.h"
#include "os/gfx.h"
#include <iostream>


#include "misc/test_main.h"

using namespace reaper;


os::gfx::Gfx* gx;

void foo()
{
	os::event::EventProxy ep = os::event::EventSystem::get_ref(0);
	os::time::Profiler prof;
	float zoom = 0.1;
	world::Sphere find_sp(Point(-3020,870,-3930), 200);
	world::Cylinder find_cyl(Point2D(0, 0), 1);
	world::Line line(Point(200, 1000, 200), Point(500, 500, 500));
	Point fru_pos(-400, 700, -400);
	Vector fru_dir(200, -300, 0);
	Vector fru_up(0, 1, 0);
	float fru_fov_w = 100;
	float fru_fov_h = 45;
	gfx::Camera cam(Point(-5000, 2000, -5000), Point(0, 0, 0), Vector(0, 1, 0), 90, 90);

	GLUquadricObj* glu_sp = gluNewQuadric();
	GLUquadricObj* glu_cyl = gluNewQuadric();

	world::WorldRef wr = world::World::get_ref();


	float z_add = 0, r_add1 = 0, r_add2 = 0;

	bool do_z_add = false, do_r_add = false;
	int obj = 0;
	while (true) {
		namespace id = os::event::id;

		if (ep.key(id::Escape))
			break;

		if (ep.key(id::Left))     cam.pos.x += 10;
		if (ep.key(id::Right))    cam.pos.x -= 10;
		if (ep.key(id::Up))       cam.pos.z += 10;
		if (ep.key(id::Down))     cam.pos.z -= 10;
		if (ep.key(id::Insert))   cam.pos.y += 10;
		if (ep.key(id::Delete))   cam.pos.y -= 10;
		if (ep.key(id::PageDown)) zoom *= 1.02;
		if (ep.key(id::PageUp))   zoom *= 0.98;

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

		switch (obj) {
		case 1:
			find_sp.p.x += dx;
			find_sp.p.y += dy;
			find_sp.p.z += dz;
			find_sp.radius += dw;
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


		if (do_z_add) {
			zoom *= 1 + ep.axis(1) - z_add;
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
		glPushMatrix();
		glScalef(zoom,zoom,zoom);

		gfx::RendererRef()->render(cam);

		world::tri_iterator c, e = wr->end_tri();

		reaper::glEnable(GL_DEPTH_TEST);
		glColor3f(1, 0, 0);
		glBegin(GL_TRIANGLES);

		prof.start();

		for (c = wr->find_tri(find_sp); c != e; ++c) {
			prof.stop();
			glVertex3f(c->a.x, c->a.y+10, c->a.z);
			glVertex3f(c->b.x, c->b.y+10, c->b.z);
			glVertex3f(c->c.x, c->c.y+10, c->c.z);
			prof.start();
		}
		prof.stop();
		glEnd();
//		prof.report();
		prof.reset();

		glColor3f(1, 1, 0);
		glBegin(GL_TRIANGLES);

		int n = 0;
		for (c = wr->find_tri(find_cyl); c != e; ++c) {
			prof.stop();
			glVertex3f(c->a.x, c->a.y+10, c->a.z);
			glVertex3f(c->b.x, c->b.y+10, c->b.z);
			glVertex3f(c->c.x, c->c.y+10, c->c.z);
			n++;
		}
		glEnd();

		glColor3f(1, 1, 1);
		glBegin(GL_TRIANGLES);

		for (c = wr->find_tri(line); c != e; ++c) {
			glVertex3f(c->a.x, c->a.y+10, c->a.z);
			glVertex3f(c->b.x, c->b.y+10, c->b.z);
			glVertex3f(c->c.x, c->c.y+10, c->c.z);
		}
		glEnd();

		glColor3f(0, 1, 1);
		glBegin(GL_TRIANGLES);

		for (c = wr->find_tri(world::Frustum(fru_pos, fru_dir, fru_up, fru_fov_w, fru_fov_h)); c != e; ++c) {
			glVertex3f(c->a.x, c->a.y+10, c->a.z);
			glVertex3f(c->b.x, c->b.y+10, c->b.z);
			glVertex3f(c->c.x, c->c.y+10, c->c.z);
		}
		glEnd();


		glColor3f(1,1,0);
		glBegin(GL_LINES);
		glVertex3fv(line.p1.get());
		glVertex3fv(line.p2.get());
		glEnd();


		glColor4f(0, 0, 0.5, 0.5);
		glPushMatrix();
		glTranslatef(find_sp.p.x, find_sp.p.y, find_sp.p.z);
		gluSphere(glu_sp, find_sp.radius, 20, 20);
		glPopMatrix();

		glPushMatrix();
		glRotatef(-90, 1, 0, 0);
		glTranslatef(find_cyl.p.x, -find_cyl.p.y, 0);
		gluCylinder(glu_cyl, find_cyl.r, find_cyl.r, 10000, 20, 20);
		glPopMatrix();

		if (true) {
			Vector left = norm(cross(fru_up, fru_dir));
			float len = length(fru_dir);
			float w = 2 * tan(3.14/180*fru_fov_w/2) * len;
			float h = 2 * tan(3.14/180*fru_fov_h/2) * len;
			Point ul = fru_pos + fru_dir + fru_up*h + left*w;
			Point ll = fru_pos + fru_dir - fru_up*h + left*w;
			Point ur = fru_pos + fru_dir + fru_up*h - left*w;
			Point lr = fru_pos + fru_dir - fru_up*h - left*w;

			glBegin(GL_TRIANGLES);
			glColor4f(1, 1, 0, 0.5);
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
		}

		glPopMatrix();


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
	gfx::RendererRef::create();
	gfx::RendererRef()->load();
	gfx::RendererRef()->start();
	gfx::RendererRef()->settings().draw_sky = false;
	gfx::RendererRef()->settings().draw_water = false;
	gfx::RendererRef()->settings().use_fog = false;
	gfx::RendererRef()->settings().draw_shadows = false;

	foo();

	return 0;
}

