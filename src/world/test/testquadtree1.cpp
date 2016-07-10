
#include "os/compat.h"

#include "os/debug.h"
#include "os/gl.h"
#include "os/gfx.h"
#include "os/event.h"
#include "misc/font.h"
#include "os/time.h"
#include "world/world.h"
#include "world/quadtree.h"
#include "main/types_io.h"
#include "main/types_ops.h"

#include "misc/test_main.h"


using namespace reaper;
using namespace quadtree;

quadtree::QuadTree<world::Triangle*> qt(0, 0, 1000);

void draw_it(world::Sphere sph, const world::Frustum& fru)
{
	static int flip = 0;
	static GLUquadricObj* gl_sph = gluNewQuadric();
	os::time::Profiler profs("sphere");
	os::time::Profiler proff("frustum");

	glPushMatrix();
	glScalef(-1, 1, 1);
	glRotatef(60, 1, 0, 0);
	glTranslatef(-500, 0, -500);
	glColor3f(0, 1, 0);
	glBegin(GL_TRIANGLES);

	world::tri_iterator c, e = qt.end();


	for (c = qt.begin(); c != e; ) {
		glVertex3fv(c->a.get());
		glVertex3fv(c->b.get());
		glVertex3fv(c->c.get());
		++c;
	}

	glEnd();

	glTranslatef(0, 5, 0);
	glColor3f(1, 0, 0);
	glBegin(GL_TRIANGLES);

	profs.start();
	c = qt.find(sph);
	profs.stop();
	::glDisable(GL_DEPTH_TEST);
	while (c != e) {
		glVertex3fv(c->a.get());
		glVertex3fv(c->b.get());
		glVertex3fv(c->c.get());
		profs.start();
		c++;
		profs.stop();
	}
	glEnd();
	profs.report();
	glColor3f(0, 0, 1);
	glBegin(GL_TRIANGLES);
	proff.start();
	c = qt.find(fru);
	proff.stop();
	int fru_n = 0;
	while (c != e) {
		glVertex3fv(c->a.get());
		glVertex3fv(c->b.get());
		glVertex3fv(c->c.get());
		proff.start();
		c++;
		proff.stop();
		fru_n++;
	}
	glEnd();
	proff.report();
	derr << fru_n << '\n';
	::glEnable(GL_DEPTH_TEST);

	glPushMatrix();
	glColor4f(0, 0, 0.5, 0.5);
	glTranslatef(sph.p.x, sph.p.y, sph.p.z);
	glRotatef(90, 1, 0, 0);
	gluSphere(gl_sph, sph.radius, 20, 20);

	glPopMatrix();


	if (true) {
		Vector left = norm(cross(fru.up(), fru.dir()));
		float len = length(fru.dir());
		float w = tan(3.14/180*fru.fov_width()/2) * len;
		float h = tan(3.14/180*fru.fov_height()/2) * len;
		Point ul = fru.pos() + fru.dir() + fru.up()*h + left*w;
		Point ll = fru.pos() + fru.dir() - fru.up()*h + left*w;
		Point ur = fru.pos() + fru.dir() + fru.up()*h - left*w;
		Point lr = fru.pos() + fru.dir() - fru.up()*h - left*w;

		glBegin(GL_TRIANGLES);
		glColor4f(0, 1, 1, 0.5);
		glVertex3fv(fru.pos().get());
		glVertex3fv(lr.get());
		glVertex3fv(ur.get());
		glColor4f(0, 0.9, 1, 0.5);
		glVertex3fv(fru.pos().get());
		glVertex3fv(ur.get());
		glVertex3fv(ul.get());
		glColor4f(0, 0.8, 1, 0.5);
		glVertex3fv(fru.pos().get());
		glVertex3fv(ul.get());
		glVertex3fv(ll.get());
		glColor4f(0, 0.7, 1, 0.5);
		glVertex3fv(fru.pos().get());
		glVertex3fv(ll.get());
		glVertex3fv(lr.get());
		
		glColor4f(0, 0.6, 1, 0.5);
		glVertex3fv(ur.get());
		glVertex3fv(ll.get());
		glVertex3fv(ul.get());

		glColor4f(0, 0.5, 1, 0.5);
		glVertex3fv(ll.get());
		glVertex3fv(ur.get());
		glVertex3fv(lr.get());
		glEnd();
	}

	glPopMatrix();


	glColor3f(1,1,1);
	char buf[100];
	snprintf(buf, 100, "x: %f  z: %f %d", sph.p.x, sph.p.z, flip);
	flip ^= 1;

}


int test_main()
{
	os::gfx::Gfx gx;
	gx.change_mode(os::gfx::VideoMode(800, 600));
	os::event::EventSystem es(gx);
	os::event::EventProxy ep = os::event::EventSystem::get_ref(0);

	float width = 11.31;
	for (float i = 400; i < 600; i+=width) {
		for (float j = 400; j < 600; j+=width) {
//			if (i < 10 ||j < 10 || i > 12 || j > 12)
//				continue;
			Point* a = new Point(i, 1, j);
			Point* b = new Point(i+width, 1, j);
			Point* c = new Point(i+width, 1, j+width);
			qt.insert(new world::Triangle(*a, *b, *c));

			Point* d = new Point(i+width, 1, j+width);
			Point* e = new Point(i, 1, j+width);
			qt.insert(new world::Triangle(*a, *d, *e));
		}
	}
	derr << "Root node size " << qt.root_elems() << '\n';
	glViewport(0,0,gx.current_mode().width, gx.current_mode().height);
	glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
	gluPerspective(70,1,1,1000);

	glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0,500,0, 0,0,0, 0,0,1);


	glClearColor(0,0,0,0);
	::glEnable(GL_DEPTH_TEST);
	::glEnable(GL_BLEND);
	::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	world::Sphere sph(Point(500, 0, 500), 15);
	world::Frustum fru(Point(550, 5, 550), Vector(20, 0, 0), Vector(0, 1, 0), 90, 45);
	float step = 10;

	Point2D p1(5, 5);
	Point2D p2(6, 7);
	while (true) {
		if (ep.key(os::event::id::Escape))
			break;
		if (ep.key(os::event::id::Right))
			sph.p.x += step;
		if (ep.key(os::event::id::Left))
			sph.p.x -= step;
		if (ep.key(os::event::id::Up))
			sph.p.z += step;
		if (ep.key(os::event::id::Down))
			sph.p.z -= step;
		if (ep.key('A'))
			sph.p.y += step;
		if (ep.key('Z'))
			sph.p.y -= step;
		if (ep.key('S'))
			sph.radius += step;
		if (ep.key('X'))
			sph.radius -= step;

		Point pos = fru.pos();
		Vector dir = fru.dir();
		Vector up = fru.up();
		float fov_w = fru.fov_width();
		float fov_h = fru.fov_height();
		if (ep.key('E')) pos.x += step;
		if (ep.key('D')) pos.x -= step;
		if (ep.key('R')) pos.y += step;
		if (ep.key('F')) pos.y -= step;
		if (ep.key('T')) pos.z += step;
		if (ep.key('G')) pos.z -= step;
		if (ep.key('Y')) dir.x += step;
		if (ep.key('H')) dir.x -= step;
		if (ep.key('U')) dir.y += step;
		if (ep.key('J')) dir.y -= step;
		if (ep.key('I')) dir.z += step;
		if (ep.key('K')) dir.z -= step;
		if (ep.key('O')) fov_w += step;
		if (ep.key('L')) fov_w -= step;
		if (ep.key('P')) fov_h += step;
		if (ep.key('M')) fov_h -= step;

		Vector new_up = norm(cross(Vector(dir.z, 0, dir.x), dir));
		fru = world::Frustum(pos, dir, new_up, fov_w, fov_h);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw_it(sph, fru);
		gx.update_screen();
		os::time::msleep(50);
		if (exit_now())
			break;
	}
	return 0;
}

