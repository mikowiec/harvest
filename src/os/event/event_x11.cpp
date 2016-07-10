
#include "os/compat.h"

#include "os/debug.h"
#include "os/event/event_prim.h"

#include <queue>

#include "misc/plugin.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "os/gfx/gfx_types.h"
#include "os/gfx/gfx_driver.h"
#include "os/interfaces.h"

#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

struct js_event {
	int time;
	short value;
	char type;
	char number;
};
namespace reaper {
namespace os {
namespace event {


const int JS_EVENT_BUTTON = 0x01;
const int JS_EVENT_AXIS   = 0x02;
const int JS_EVENT_INIT   = 0x80;

using lowlevel::Gfx_driver_data;

class Keyboard : public InputDevice {

	id::EventID translate_key(unsigned int key) {
		switch (key) {
		case XK_Return:	return id::Enter;
		case XK_Down:	return id::Down;
		case XK_Up:	return id::Up;
		case XK_Left:	return id::Left;
		case XK_Right:	return id::Right;
		case XK_Escape:	return id::Escape;
		case XK_Prior:  return id::PageUp;
		case XK_Next:   return id::PageDown;
		case XK_Delete: return id::Delete;
		case XK_Insert: return id::Insert;
		case XK_BackSpace: return id::Backspace;
		case XK_End:	return id::End;
		case XK_Home:	return id::Home;
		case XK_F1:	return id::F1;
		case XK_F2:	return id::F2;
		case XK_F3:	return id::F3;
		case XK_F4:	return id::F4;
		case XK_F5:	return id::F5;
		case XK_F6:	return id::F6;
		case XK_F7:	return id::F7;
		case XK_F8:	return id::F8;
		case XK_F9:	return id::F9;
		case XK_F10:	return id::F10;
		case XK_F11:	return id::F11;
		case XK_F12:	return id::F12;
		}
		if (key < 256)
			return id::mk(toupper(key));
		else
			return id::Unknown;
	}
	const Gfx_driver_data* gx;
	ifs::Event* main;
protected:
	bool poll_device()
	{
		XEvent e;
		if (XCheckMaskEvent(gx->display, KeyPressMask |
						 KeyReleaseMask, &e)) {
			KeySym key;
			char buf[10];
			XLookupString((XKeyEvent*)&e, buf, 10, &key, 0);
			Event ev(translate_key(key), e.type == KeyPress);
			pending.push(ev);
		}
		return !pending.empty();
	}
public:
	Keyboard(const Gfx_driver_data* g, ifs::Event* m)
	 : InputDevice(), gx(g), main(m) { }
	~Keyboard() { }
};

class Mouse : public InputDevice {
	ifs::Event* main;
	const Gfx_driver_data* gx;
protected:
	bool poll_device();
public:
	Mouse(const Gfx_driver_data* g, ifs::Event* m) : main(m), gx(g) { }
	~Mouse() { }
};


bool Mouse::poll_device()
{
	int x_res = gx->current.width;
	int y_res = gx->current.height;

	XEvent e;
	if (XCheckMaskEvent(gx->display, ButtonPressMask |
					 ButtonReleaseMask |
					 PointerMotionMask, &e)) {
		XButtonEvent* eb = (XButtonEvent*)&e;
		if (e.type == MotionNotify) {
			float x_r = norm(eb->x, x_res);
			float y_r = norm(eb->y, y_res);
			pending.push(Event(0, x_r));
			pending.push(Event(1, y_r));
		} else {
			int b = eb->button;
			if (b & 1)
				b >>= 1;
			pending.push(Event(b, e.type == ButtonPress));
		}
	}
	return !pending.empty();
}

class Joystick : public InputDevice {
	ifs::Event* main;
	bool debug;
	enum { x_axis = 2, y_axis = 3 };
	std::deque<int> axis_val;
	int fd;
	void read_axis(int a, int n);
protected:
	bool poll_device();
public:
	Joystick(ifs::Event* m) : main(m), debug(false) {
		axis_val.resize(main->num_def_axis(), 0);
	}
	~Joystick() { }
	bool init()
	{
		fd = open("/dev/js0", O_RDONLY | O_NONBLOCK);
		debug = main->config("joystick_debug") == "true";
		return (fd > 0);
	}
};


void Joystick::read_axis(int a, int new_val)
{
	if (has_changed(main->axis_config(a), axis_val[a], new_val)) {
		axis_val[a] = new_val;
		float v = norm(new_val, main->axis_config(a));
		pending.push(Event(a, v));
	}


}

bool Joystick::poll_device()
{
	if (fd < 0) 
		return false;
	struct js_event jev;
	int axis;
	int i = 10;
	while (--i && read(fd, &jev, sizeof(jev)) > 0) {
		if (jev.type & JS_EVENT_INIT) {
			jev.type &= ~JS_EVENT_INIT;
//			main->derr() << "init for "
//			             << ((jev.type == JS_EVENT_BUTTON) ? "button" : "axis")
//				     << " value: " << jev.value << '\n';
		}
		switch (jev.type) {
		case JS_EVENT_BUTTON:
			pending.push(Event(3 + jev.number, bool(jev.value)));
			break;
		case JS_EVENT_AXIS:
			int npos = jev.value + 32768;
			read_axis(x_axis + jev.number, npos);
			break;
		}
	}
	return !pending.empty();
}


class IDMX11
 : public InputDeviceModule
{
	ifs::Event* main;
public:
	IDMX11(ifs::Event* m) : main(m) { }
	void scan_inputdevices(Gfx_driver_data* g, device_inserter out)
	{
		*out++ = new Keyboard(g, main);
		*out++ = new Mouse(g, main);
		if (main->config("use_joystick") == "true") {
			Joystick* js = new Joystick(main);
			if (js->init())
				*out++ = js;
			else
				delete js;
		}
	}
};

}
}
}

extern "C" {
DLL_EXPORT void* create_event_x11(reaper::os::ifs::Event* m)
{
	return new reaper::os::event::IDMX11(m);
}
}


#ifdef REAPER_EMBED_PLUGINS
namespace {
struct RegisterPlugin {
	RegisterPlugin() {
		reaper::misc::preloaded::add_plugin_creator("event_x11", (reaper::misc::VoidCreator1)create_event_x11);
	}
} foo;
}
#endif

