
#include "os/compat.h"

#include "os/gfx.h"
#include "os/gl.h"
#include "os/event.h"
#include "os/mapping.h"
#include "os/debug.h"
#include "os/time.h"

#include "misc/test_main.h"

using namespace reaper;

using os::event::EventSystem;
using os::event::EventProxy;
using os::event::Event;

int test_main()
{
	debug::ExitFail ef(1);  // Return 1 if an exception is thrown.
	os::gfx::Gfx gx;	    // Get a handle to the gfx-system
//	gx.change_mode(os::gfx::VideoMode(640, 480));

	EventSystem es(gx);	// Get a handle to the event-system
	EventProxy ep = EventSystem::get_ref(0);
	os::event::Event ev;

	glViewport(0,0,gx.current_mode().width, gx.current_mode().height);		// hela fönstret
	glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
	gluPerspective(70,1,1,1000);

	glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(0,0, -1.7,0,0,0,0,1,0);
	::glClearColor(0,0,0,0);

	GLUquadricObj* pt = gluNewQuadric();
	os::time::TimeSpan start = os::time::get_time();
	try {
		float x_pos = 0;
		float y_pos = 0;
		float x_j_pos = 0;
		float y_j_pos = 0;
		float x_j2_pos = 0;
		float y_j2_pos = 0;
		while (true) {

			glClear(GL_COLOR_BUFFER_BIT);
			glColor3f(1,1,1);
			glRectf(-1, -1, 1, 1);
			glColor3f(0.8,0.8,0.8);
			glBegin(GL_LINES);
			for (int i = -1; i <= 1; i++) {
				glVertex2f( i,-1);
				glVertex2f( i, 1);
				glVertex2f(-1, i);
				glVertex2f( 1, i);
			}
			glEnd();

			glPushMatrix();
			glTranslatef(-x_pos, -y_pos, 0);
			glColor3f(1,ep.button(0), ep.button(1));
			gluSphere(pt, 0.02, 20, 20);
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-x_j_pos, -y_j_pos, 0);
			glColor3f(0,0,1);
			gluSphere(pt, 0.02, 20, 20);
			glPopMatrix();

			glPushMatrix();
			glTranslatef(-x_j2_pos, -y_j2_pos, 0);
			glColor3f(0,1,1);
			gluSphere(pt, 0.02, 20, 20);
			glPopMatrix();

			gx.update_screen();

			// Exit on escape.
			if (ep.key(os::event::id::Escape))
				break;
			x_pos = ep.axis(0);
			y_pos = ep.axis(1);
			x_j_pos = ep.axis(2);
			y_j_pos = ep.axis(3);
			x_j2_pos = ep.axis(4);
			y_j2_pos = ep.axis(5);
			if (ep.key(' '))
				es.reconfigure();
			if (exit_now())
				break;
		}

	} catch (os::hw_fatal_error& e) {
		derr << e.what() << '\n';
		return 2;
	}
	ef.disable();
	return 0;
}

