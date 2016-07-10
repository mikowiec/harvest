
/*
 * $Author: pstrand $
 * $Date: 2003/03/28 13:47:27 $
 * $Log: menu_system.cpp,v $
 * Revision 1.22  2003/03/28 13:47:27  pstrand
 * better handling of resize
 *
 * Revision 1.21  2003/01/06 12:42:07  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.20  2002/12/29 17:04:39  pstrand
 * hw/ moved to os/
 *
 * Revision 1.19  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.18  2002/02/26 22:33:49  pstrand
 * mackefix
 *
 * Revision 1.17  2002/01/23 04:42:51  peter
 * mackes gfx-fix in igen, och fix av dylik... ;)
 *
 * Revision 1.16  2002/01/22 23:44:06  peter
 * reversed last two revs
 *
 * Revision 1.14  2002/01/21 23:29:32  peter
 * *** empty log message ***
 *
 * Revision 1.13  2002/01/21 00:20:59  peter
 * musikfixar, mm..
 *
 * Revision 1.12  2002/01/17 05:04:02  peter
 * mp3 i meny (inte igång men funkar), pluginfix..
 *
 * Revision 1.11  2002/01/11 21:30:21  peter
 * *** empty log message ***
 *
 * Revision 1.10  2002/01/10 23:09:13  macke
 * massa bök
 *
 * Revision 1.9  2002/01/10 22:30:27  peter
 * resursfixar.. mm..
 *
 * Revision 1.8  2002/01/07 14:00:31  peter
 * resurs och ljudmeck
 *
 * Revision 1.7  2002/01/01 23:25:21  peter
 * musik
 *
 * Revision 1.6  2001/12/24 14:40:30  peter
 * 0.93
 *
 * Revision 1.5  2001/12/15 17:47:23  peter
 * menymeck..
 *
 * Revision 1.4  2001/12/14 16:13:36  macke
 * grmbl
 *
 * Revision 1.3  2001/12/14 16:10:50  macke
 * powermenus!
 *
 * Revision 1.2  2001/12/11 21:25:43  macke
 * menu.cpp
 *
 * Revision 1.1  2001/12/11 18:57:22  macke
 * bök
 *
 * Revision 1.42  2001/12/11 18:46:40  macke
 * Menusystem refactored..
 *
 * Revision 1.41  2001/12/04 23:01:24  picon
 * menu system now working ... barely. :)
 *
 * Revision 1.40  2001/11/26 02:20:13  peter
 * ljudfixar (smartptr), andra mindre fixar..
 *
 * Revision 1.39  2001/08/20 17:07:51  peter
 * *** empty log message ***
 *
 * Revision 1.38  2001/07/27 15:48:35  peter
 * gfx..
 *
 * Revision 1.37  2001/06/07 05:14:24  macke
 * Reaper v0.9
 *
 *
 *
 */

#include "os/compat.h"

#include <cmath>

#include "os/gl.h"
#include "msg/event.h"
#include "os/debug.h"
#include "msg/mapping.h"
#include "os/snd.h"
#include "os/gfx.h"
#include "gfx/texture.h"
#include "misc/font.h"
#include "misc/free.h"
#include "misc/menu_system.h"

#include "gfx/settings.h"
#include "gfx/abstracts.h"
#include "gfx/initializers.h"

namespace reaper
{
namespace menu
{

using reaper::os::event::Event;
using reaper::gfx::font::glPutStr;
namespace id = os::event::id;

namespace {
reaper::debug::DebugOutput derr("menu");
}

////////////////////////////////////////////////////////////////////////////////

class menu_map
{
	bool lock;
public:
	menu_map() : lock(false) { }
	Event operator()(const Event& e)
	{
		Event ev(e);
		if (ev.id == id::Axis0 || ev.id == id::Axis1) {
			ev.val = (ev.val + 1.0) / 2.0;
			return ev;
		} 
		if (ev.id == id::Axis3) {
			if (ev.val < -0.1) {
				if (! lock && ev.val < -0.8) {
					lock = true;
					return Event(id::Up, true);
				}
				if (lock && ev.val > -0.6) {
					lock = false;
					return Event(id::Up, false);
				}
			}
			if (ev.val > 0.1) {
				if (! lock && ev.val > 0.8) {
					lock = true;
					return Event(id::Down, true);
				}
				if (lock && ev.val < 0.6) {
					lock = false;
					return Event(id::Down, false);
				}
			}
		}
		//derr << e.val << ' ' << e.id << '\n';
		if (e.val > 0.5 && (e.is_key() || e.is_btn()))
			return e;
		return Event(id::Unknown, true);
	}
};

////////////////////////////////////////////////////////////////////////////////

class MenuSystem::Impl
{
	os::gfx::Gfx gx;
	os::event::EventSystem es;
	os::snd::SoundSystem sm;

	os::snd::SoundPtr mp3;
	os::snd::EffectPtr select;
	os::snd::EffectPtr press;
	std::auto_ptr<os::event::EventProxy> ep;
	gfx::texture::Texture mouse_ptr;
public:
	Impl();
	~Impl();

	void draw_background(Texture&);
	void draw_mouse_ptr(float x, float y);

	os::event::EventProxy& event();
	os::gfx::Gfx& gfx();

	void sel_snd();
	void press_snd();

	void do_stuff() {
		glViewport(0, 0, gx.current_mode().width,
				 gx.current_mode().height);
		sm.do_stuff();
	}
};


MenuSystem::Impl::Impl()
: gx(), es(gx), sm(), mp3(0),
  ep(0), mouse_ptr("arrow")
{
	mp3 = sm.prepare_music("reaper.small");
	sm.init();
	select = sm.prepare_effect("menu_select");
	press = sm.prepare_effect("menu_press");
	using namespace os::event;
	es.set_mapping(create_mapping(menu_map()));
	ep = std::auto_ptr<EventProxy>(new EventProxy(EventSystem::get_ref(0)));

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glViewport(0, 0, gx.current_mode().width,
			 gx.current_mode().height);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, 1, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gfx::initializers::font_init->init();
	if (mp3.get())
		mp3->play();
}

MenuSystem::Impl::~Impl()
{
	gfx::initializers::font_init->exit();
	if (mp3.get()) {
		mp3->stop();
	}
}

void MenuSystem::do_stuff()
{
	i->do_stuff();
}

void MenuSystem::Impl::draw_background(Texture& back) {
	draw_texture(Box(0,0, 1.0, 1.0), back);
}

void MenuSystem::Impl::draw_mouse_ptr(float x, float y)
{
	glColor3f(1,1,1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	draw_texture(Box(x-.005, y, x+.04, y+.035), mouse_ptr);
	glDisable(GL_BLEND);
}

os::event::EventProxy& MenuSystem::Impl::event() {
	return *ep; 
}

os::gfx::Gfx& MenuSystem::Impl::gfx() {
	return gx;
}

void MenuSystem::Impl::sel_snd() {
	if (select.get())
		select->play();
}

void MenuSystem::Impl::press_snd() {
	if (press.get())
		press->play();
}

////////////////////////////////////////////////////////////////////////////////

MenuSystem::MenuSystem() : i(0) {
	gfx::Settings::current = gfx::Settings::defaults;
	i = new Impl();
}

MenuSystem::~MenuSystem()
{
	delete i;
}

void MenuSystem::draw_background(Texture& t)      { i->draw_background(t); }
void MenuSystem::draw_mouse_ptr(float x, float y) { i->draw_mouse_ptr(x,y); }
os::event::EventProxy& MenuSystem::event()        { return i->event(); }
os::gfx::Gfx& MenuSystem::gfx()                   { return i->gfx(); }
void MenuSystem::sel_snd()   { i->sel_snd(); }
void MenuSystem::press_snd() { i->press_snd(); }

////////////////////////////////////////////////////////////////////////////////
// global funcs

void draw_texture(Box b, Texture& t)
{
	t.use();
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(b.x1, 1.0 - b.y1);
		glTexCoord2f(1, 0);
		glVertex2f(b.x2, 1.0 - b.y1);
		glTexCoord2f(1, 1);
		glVertex2f(b.x2, 1.0 - b.y2);
		glTexCoord2f(0, 1);
		glVertex2f(b.x1, 1.0 - b.y2);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void draw_text_box(Box b, const std::string& s, bool active, bool passive, bool editing)
{
	glPushMatrix();
		float w = b.x2 - b.x1;
		float h = b.y2 - b.y1;
		float x_c = b.x1 + w / 2;
		float y_c = b.y1 + h / 2;

		glTranslatef(x_c, 1.0 - y_c, 0.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glColor4f(0.0, 0.0, 0.0, active ? 0.2 : 0.7);
		glRectf(-w/2, -h/2, w/2, h/2);

		glColor3f(passive ? 1 : 0, 1, 1);
		float center_adj = (s.length() / 2.0) * 0.01;
		glPutStr(-center_adj, -h/8, s.c_str(), gfx::font::Large, 0.01, 0.02);

		glDisable(GL_BLEND);
		if(active) {
			if(editing) {
				glColor3f(0,1,1);
			} else {
				glColor3f(.5,.5,.5);
			}
			glBegin(GL_LINE_LOOP);
			glVertex2f(-w/2, -h/2);
			glVertex2f(w/2, -h/2);
			glVertex2f(w/2, h/2);
			glVertex2f(-w/2, h/2);
			glEnd();
		}
	glPopMatrix();
}

void draw_header(Box b, const std::string &s)
{
	float w = b.x2 - b.x1;
	float h = b.y2 - b.y1;
	float cw = w / s.length();

	glColor3f(1,1,1);
	glPutStr(b.x1, 1 - b.y2, s.c_str(), gfx::font::Large, cw, h);
}

}
}

