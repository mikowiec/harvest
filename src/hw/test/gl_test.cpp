
#include "os/compat.h"

#include <vector>
#include <list>
#include <deque>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <sstream>
#include <cmath>
#include <strstream>

#include "gfx/main.h"


#include "misc/sequence.h"
#include "os/gfx.h"
#include "os/event.h"

#include "misc/menu.h"
#include "os/time.h"
#include "os/debug.h"
#include "main/exceptions.h"
#include "os/gl.h"
#include "res/res.h"
#include "os/mapping.h"
#include "misc/sequence.h"
#include "os/snd.h"

#include "os/socket.h"

#include "os/test/pong.h"

using namespace reaper::res;
using namespace reaper::misc;
using namespace reaper::hw;
using namespace reaper::os::gfx;
using namespace reaper::os::snd;
using namespace reaper::os::event;
using namespace reaper::os::time;

namespace { reaper::os::debug::DebugOutput derr("gl_test", 1); }

enum Test { None = 1 };

struct print_mode : public std::unary_function<void, VideoMode> {
	void operator()(const VideoMode& vm) {
		derr << "Avail: " << vm << std::endl;
	}
};


namespace {
	const float rotspeed = 5;
}

float angle = 0.0;
int texture[512*512];
GLuint texnames[1];

void init_stuff(const VideoMode& mode) {
	glClearColor(0,0,0,0);
	glViewport(0, 0, mode.width, mode.height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90,1,1,20);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0,0,-3, 0,0,0, 0,1,0);

}

void draw_stuff() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	static int j = 0; j++;
	angle += rotspeed;
	float a = angle/90;
	glPushMatrix();   
	glRotatef(angle,0,0,1);

	glBegin(GL_TRIANGLES);
	glColor3f(cos(a+2),sin(a),-cos(a+3));
	glVertex3f(0.0f ,2.0f, -0.1f);
	glColor3f(cos(a),-sin(a+1),cos(a+2));
	glVertex3f(-2.0f,-1.0f,-0.1f);
	glColor3f(-sin(a+2),cos(a),-sin(a+1));
	glVertex3f(2.0f,-1.0f,-0.1f);
	glEnd();

	glPopMatrix();


}

using namespace reaper::menu;


typedef MenuItem<int> MI;

template<class Iter>
class mk_menu : public std::unary_function<void, const VideoMode&>
{
	Iter iter;	
	float x, y;
	float xstep, ystep;
	int count;
public:
	mk_menu(Iter i, float xx, float yy, float xs, float ys)
	 : iter(i), x(xx), y(yy), xstep(xs), ystep(ys), count(0)
	{ }
	void operator()(const VideoMode& mode)
	{
		std::string label;
		std::ostringstream ls(label);
		ls << mode.width << ' ' << mode.height << ' ' << mode.depth;
		*iter++ = MI(Box(x, y, x+0.4, y+0.08), ls.str(), count++);
		x += xstep;
		y += ystep;
	}
};


VideoMode run_menu(Gfx& gfx, Events& ev, reaper::menu::Texture& back)
{
	std::deque<MI> menu;
	std::vector<VideoMode> modes;

	gfx->get_modes(std::back_inserter(modes));

	// Yuck!
	std::for_each(modes.begin(), modes.end(), mk_menu<std::back_insert_iterator<std::deque<MI> > >
			(std::back_inserter(menu), 0.2f, 0.2f, 0.0f, 0.07f));

	menu_begin(gfx, &ev);
	int i = run_menu<std::deque<MI>::iterator, int>(gfx, &ev, -1, 
			 menu.begin(), menu.end(), back);
	if (i == -1)
		return gfx->current_mode();
	else
		return modes[i];
}

void loop() {
	timespan t1;
	bool cont = true;
	try {
		Gfx gfx;
		gfx->change_mode(VideoMode(800, 600, false));
		EventTable ev(gfx);
//		SoundSystem snd(gfx);

//		reaper::gfx::main::Texture back("test_main");
//		VideoMode v = run_menu(gfx, e, back);

//		if (!(v == gfx->current_mode()))
//			gfx->change_mode(v);

		Pong pong;
		pong.resize(gfx->current_mode().width,
			    gfx->current_mode().height);
		pong.redraw();
		t1 = get_time();
		do {
			if (ev.key(event::Up))
				pong.cmd(0, ::Left);
			if (ev.key(event::Down))
				pong.cmd(0, ::Right);
			if (ev.key(event::Left))
				pong.cmd(1, ::Left);
			if (ev.key(event::Right))
				pong.cmd(1, ::Right);

			if (ev.key(event::Escape))
				cont = false;
/*
			e.poll_inputs();
			Event ev;
			while (e.get_event(ev)) {
				if (! ev.is_key() || ev.val < 0.5)
					continue;
				switch (ev.key) {
				case event::Escape:
					cont = false;
					break;
				case event::Down:
					pong.move(0, -1);
					break;
				case event::Up:
					pong.move(0, 1);
					break;
				default: break;
				}
			}
*/
			if (pong.tick()) {
				pong.redraw();
				gfx->update_screen();
			}
		} while (cont && !pong.finished());
		gfx->restore();
	} catch (reaper::fatal_error_base& e) {
		derr << "Fatal: " << e.what() << '\n';
	} catch (reaper::error_base& e) {
		derr << "Error: " << e.what() << '\n';
	} catch (std::string& s) {
		derr << "UGH! Exception (string): " << s << '\n';
	} catch (const char* s) {
		derr << "UGH! Exception (char*): " << s << '\n';
	} catch (std::exception& e) {
		derr << "Exception: " << e.what() << '\n';
	} catch (...) {
		derr << "UGH! unknown exception" << '\n';
	}
}

void vc_suger()
{
	std::istrstream s((char*)0);
	int i;
	s >> i;
}

int main(int, char**) {
	loop();
	return 0;
}



