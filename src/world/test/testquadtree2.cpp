
#include "os/compat.h"

#include "os/debug.h"
#include "os/gl.h"
#include "os/gfx.h"
#include "os/event.h"
#include "misc/font.h"
#include "os/time.h"
#include "world/world.h"
#include "world/quadtree.h"

#include "misc/test_main.h"

using namespace reaper;
using namespace quadtree;

QuadTree<world::Triangle*> qt(0, 0, 1000);

void draw_it(const world::Line& line)
{
	glPushMatrix();
	glRotatef(40, 1, 0, 0);
	glTranslatef(-300, 0, -300);

	::glDisable(GL_DEPTH_TEST);
	glColor3f(0, 1, 0);
	glBegin(GL_TRIANGLES);

	world::tri_iterator c, e = qt.end();
	for (c = qt.begin(); c != e; ++c) {
		glVertex3fv(c->a.get());
		glVertex3fv(c->b.get());
		glVertex3fv(c->c.get());
	}
	glEnd();

	glColor3f(0, 0, 1);
	glBegin(GL_TRIANGLES);
	for (c = qt.find(line); c != e; ++c) {
		glVertex3fv(c->a.get());
		glVertex3fv(c->b.get());
		glVertex3fv(c->c.get());
	}
	glEnd();

	::glEnable(GL_DEPTH_TEST);
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	glVertex3fv(line.p1.get());
	glVertex3fv(line.p2.get());
	glEnd();

	glPopMatrix();

	glColor3f(1,1,1);
}


int test_main()
{
	os::gfx::Gfx gx;
	gx.change_mode(os::gfx::VideoMode(800, 600));
	reaper::debug::ExitFail ef(1);
	os::event::EventSystem es(gx);
	os::event::EventProxy ep = os::event::EventSystem::get_ref(0);

	qt.insert(new world::Triangle(
				Point(200, 0, 200),
				Point(500, 1, 200),
				Point(500, 1, 500)
				));
	qt.insert(new world::Triangle(
				Point(500, 10, 500),
				Point(700, 10, 500),
				Point(700, 10, 700)
				));

	glViewport(0,0,gx.current_mode().width, gx.current_mode().height);		// hela fönstret
	glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
	gluPerspective(70,1,1,1000);

	glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0,500,0, 0,0,0, 0,0,1);

	glClearColor(0,0,0,0);
	::glEnable(GL_DEPTH_TEST);

	world::Line line(Point(500, 100, 500), Point(480, -10, 490));
	float step = 1;
	os::time::TimeSpan start = os::time::get_time();

	while (true) {
		if (ep.key(os::event::id::Escape))
			break;
		if (ep.key('1')) line.p1.x += 10.0;
		if (ep.key('2')) line.p1.x -= 10.0;
		if (ep.key('3')) line.p1.y += 10.0;
		if (ep.key('4')) line.p1.y -= 10.0;
		if (ep.key('5')) line.p1.z += 10.0;
		if (ep.key('6')) line.p1.z -= 10.0;

		if (ep.key('7')) line.p2.x += 10.0;
		if (ep.key('8')) line.p2.x -= 10.0;
		if (ep.key('9')) line.p2.y += 10.0;
		if (ep.key('0')) line.p2.y -= 10.0;
		if (ep.key('-')) line.p2.z += 10.0;
		if (ep.key('=')) line.p2.z -= 10.0;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		draw_it(line);
		gx.update_screen();
		os::time::msleep(50);
		if (exit_now())
			break;
	}

	ef.disable();
	return 0;
}

