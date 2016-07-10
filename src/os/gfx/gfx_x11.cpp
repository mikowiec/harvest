


#include "os/compat.h"

#include <string>

#include "misc/prim_iter_impl.h"

#include "misc/plugin.h"

#include "os/gfx.h"
#include "os/gfx/gfx_driver.h"
#include "os/gfx/gfx_types.h"
#include "os/debug.h"

#include "os/interfaces.h"
#include "os/exceptions.h"

#include "os/gl.h"

#include <math.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

// undef:ed in os/gl.h
#ifndef None
#define None 0L
#endif

#include <GL/glx.h>

#ifdef XF86VM_EXT
#include <X11/extensions/xf86vmode.h>
#endif




namespace reaper
{
namespace os
{
namespace lowlevel
{


using std::string;


struct MotifWmHints {
	long flags;
	long functions;
	long decorations;
	long input_mode;
};
const int MWM_HINTS_DECORATIONS = 2;

#ifdef XF86VM_EXT
typedef XF86VidModeModeInfo video_mode;
#else
typedef int video_mode;
#endif

class Gfx_x11
 : public Gfx_driver
{
	Display* display;
	int screen;
	Window root;
	Window back_window;
	Window window;
	XVisualInfo* visual;
	GC gc;
	Colormap colormap;
	GLXContext glxcontext;
	int num_modes;
	bool fullscreen;
	video_mode** int_modes;
	Gfx_driver_data data;
	ifs::Gfx* main;
	bool nograb;
	std::set<gfx::VideoMode> vmodes;

	void fix_scr(int width, int height);
public:
	Gfx_x11(ifs::Gfx* m);
	~Gfx_x11();
	Gfx_driver_data* internal_data() { return &data; }
	bool update_screen();
	bool setup_mode(const gfx::VideoMode&);
	void restore_mode();

	misc::prim_iter_if<gfx::VideoMode>* modes() const;

};

Gfx_x11::Gfx_x11(ifs::Gfx* m)
 : display(0), screen(0), visual(0), main(m), nograb(false)
{

	if (! (display = X11::XOpenDisplay(0))) {
		throw hw_fatal_error("Cannot open display: " +
				     string(X11::XDisplayName(0)));
	}
	data.display = display;
	screen = DefaultScreen(display);
	root = XRootWindow(display, screen);
	back_window = window = 0;

	data.desktop.width  = DisplayWidth(display, screen);
	data.desktop.height = DisplayHeight(display, screen);
#ifdef XF86VM_EXT
	data.desktop.fullscreen = true;
	XF86VidModeGetAllModeLines(display, screen, &num_modes, &int_modes);

	for (int i = 0; i < num_modes; i++) {
		gfx::VideoMode mode( int_modes[i]->hdisplay,
				int_modes[i]->vdisplay, true);
		vmodes.insert(mode);
	}
#else
	data.desktop.fullscreen = false;
	num_modes = 0;
#endif
	nograb = main->config("nograb") == "true";
	main->derr() << nograb << " initialized\n";
}

Gfx_x11::~Gfx_x11()
{
	main->derr() << "shutdown\n";
	if (visual)
		XFree(visual);
	if (glxcontext)
		glXDestroyContext(display, glxcontext);
}


int attribs[][7] = {
	{ GLX_RGBA, GLX_ALPHA_SIZE, 8, GLX_STENCIL_SIZE, 8, GLX_DOUBLEBUFFER, None },
	{ GLX_RGBA, GLX_ALPHA_SIZE, 8, GLX_DOUBLEBUFFER, None, 0,0 },
	{ GLX_RGBA, GLX_DOUBLEBUFFER, None, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0 }
};
        


bool Gfx_x11::setup_mode(const gfx::VideoMode& mode)
{
	visual = 0;
	for (int i = 0; attribs[i][0] && !visual; i++) {
		data.alpha = attribs[i][1] == GLX_ALPHA_SIZE;
		data.stencil = attribs[i][3] == GLX_STENCIL_SIZE;
		visual = glXChooseVisual(display, screen, attribs[i]);
	}
	if (!visual)
		throw hw_error("glXChooseVisual failed");


	glxcontext = glXCreateContext(display, visual, 0, 1);
	if (!glxcontext)
		throw hw_error("glXCreateContext error");

	XSetWindowAttributes attr;
	attr.background_pixel = BlackPixel(display, screen);
	attr.colormap = XCreateColormap(display, root,
					visual->visual, AllocNone);
	unsigned long vmask = CWBackPixel | CWColormap;

	long eventmask  = KeyPressMask | KeyReleaseMask | StructureNotifyMask
			| ExposureMask | PointerMotionMask | FocusChangeMask
			| ButtonPressMask | ButtonReleaseMask;

	if (mode.fullscreen) {
		vmask |= CWOverrideRedirect;
		attr.override_redirect = 1;
		back_window = root;
	} else {
		back_window = XCreateWindow(
				display, root, 0, 0,
				mode.width, mode.height, 0,
				visual->depth, InputOutput, 
				visual->visual, vmask, &attr);
		XSelectInput(display, back_window, eventmask);
		XMapWindow(display, back_window);
	}
	window = XCreateWindow(display, back_window, 0, 0,
			mode.width, mode.height, 0, 
			visual->depth, InputOutput, 
			visual->visual, vmask, &attr);

	XSelectInput(display, window, eventmask);

	Pixmap pix;
	XColor dmy;
	char dt[1] = {0};
	Cursor cursor;
	pix = XCreateBitmapFromData(display, window, dt, 1, 1);
	cursor = XCreatePixmapCursor(display, pix, pix, &dmy, &dmy, 0, 0);
	XFreePixmap(display, pix);

	XDefineCursor(display, window, cursor);

	glXMakeCurrent(display, window, glxcontext);
	glXWaitX();
	XMapWindow(display, window);

	if (mode.fullscreen) {

		Atom motifhints = XInternAtom(display, "_MOTIF_WM_HINTS", 0);
		MotifWmHints wmhints;
		wmhints.flags = MWM_HINTS_DECORATIONS;
		wmhints.decorations = 0;
		XChangeProperty(display, window, motifhints,
				motifhints, 32, PropModeReplace,
				(unsigned char*) &wmhints, 4);


#ifdef XF86VM_EXT
		XF86VidModeSwitchToMode(display, screen, int_modes[mode.id]);
		XF86VidModeSetViewPort(display, screen, 0, 0);
#endif



		XGrabPointer(display, window, True, 0, 
			      GrabModeAsync, GrabModeAsync, window, 
			      None, CurrentTime);

		XGrabKeyboard(display, window, True,
			      GrabModeAsync, GrabModeAsync, CurrentTime);
	}
	fullscreen = mode.fullscreen;
	data.current = mode;

	const char *pr = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	data.is_accelerated = pr && (strstr(pr, "Mesa") == 0);

	return true;
}

void Gfx_x11::fix_scr(int width, int height)
{
	using gfx::VideoMode;
	bool fs = data.current.fullscreen;

	int new_w  = static_cast<int>(height*4.0/3) - 2;
	int new_wh = static_cast<int>(new_w*3.0/4);
	int new_h  = static_cast<int>(width*3.0/4) - 2;
	int new_hw = static_cast<int>(new_h*4.0/3);
	VideoMode vm = (new_w < width)
		? VideoMode(new_w, new_wh, fs)
		: VideoMode(new_hw, new_h, fs);
	main->derr() << "new size: " << new_w << 'x' << new_wh << '\n';
	main->derr() << "      or: " << new_hw << 'x' << new_h << '\n';
	main->derr() << "choosing: " << vm.width << 'x' << vm.height << '\n';


	XResizeWindow(display, window, vm.width, vm.height);
	XMoveWindow(display, window, (width - vm.width) / 2,
				     (height - vm.height) / 2);
	XRaiseWindow(display, window);
	data.current = vm;
	glXWaitX();
	glXSwapBuffers(display, window);
}

void Gfx_x11::restore_mode() {
	if (window) {
		if (fullscreen) {
			XUngrabPointer(display, CurrentTime);
			XUngrabKeyboard(display, CurrentTime);
#ifdef XF86VM_EXT
			XF86VidModeSwitchToMode(display, screen, int_modes[0]);
#endif
			glXWaitX();
		}
		glXMakeCurrent(display, None, 0);
		XUnmapWindow(display, window);
		XDestroyWindow(display, window);
		if (back_window != root) {
			XUnmapWindow(display, back_window);
			XDestroyWindow(display, back_window);
		}
		XEvent ev;
		while (XCheckMaskEvent(display, StructureNotifyMask, &ev))
			;
	}
	fullscreen = false;
}

bool Gfx_x11::update_screen() {
	bool resized = false;
	XEvent ev;
	while (XCheckMaskEvent(display, FocusChangeMask|StructureNotifyMask|ExposureMask, &ev)) {
		switch (ev.type) {
//		case MotionNotify:    main->derr() << "MotionNotify\n";    break;
//		case Expose: 	      main->derr() << "Expose\n";          break;
//		case UnmapNotify:     main->derr() << "UnmapNotify\n";     break;
		case FocusIn:
		case MapNotify:
			if (!nograb) {
				XGrabPointer(display, window, True, 0, 
					GrabModeAsync, GrabModeAsync, window, 
					None, CurrentTime);
			}
//			main->derr() << "MapNotify " << ev.xmap.window << "\n";
			break;
		case FocusOut:
			XUngrabPointer(display, CurrentTime);
			break;
		case MapRequest: break;
		case ConfigureNotify:
/*
			main->derr() << "ConfigureNotify: " << ev.xconfigure.window << ' '
				     << ev.xconfigure.width << "x"
				     << ev.xconfigure.height << '\n';
*/
			if (ev.xconfigure.window == back_window)
				fix_scr(ev.xconfigure.width, ev.xconfigure.height);
			resized = true;
			break;
//		case ReparentNotify:  main->derr() << "ReparentNotify\n";  break;
		default:
//			main->derr() << "msg: " << ev.type << '\n';
			break;
		}
	}
	glXSwapBuffers(display, window);
	return resized;
}

misc::prim_iter_if<gfx::VideoMode>* Gfx_x11::modes() const
{
	return misc::mk_iter(vmodes);
}

}
}
}

extern "C" {
void* create_gfx_x11(reaper::os::ifs::Gfx* m)
{
	return new reaper::os::lowlevel::Gfx_x11(m);
}
}


#ifdef REAPER_EMBED_PLUGINS
namespace {
struct RegisterPlugin {
	RegisterPlugin() {
		reaper::misc::preloaded::add_plugin_creator("gfx_x11", (reaper::misc::VoidCreator1)create_gfx_x11);
	}
} foo;
}
#endif
