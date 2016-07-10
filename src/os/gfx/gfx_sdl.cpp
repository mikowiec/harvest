
/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:13 $
 * $Log: gfx_sdl.cpp,v $
 * Revision 1.2  2003/01/06 12:42:13  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.5  2001/08/06 12:16:21  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.4.2.1  2001/07/31 17:32:09  peter
 * testgren...
 *
 * Revision 1.4  2001/07/27 15:47:34  peter
 * massive reorg...
 *
 * Revision 1.3  2001/07/19 00:38:26  peter
 * *** empty log message ***
 *
 * Revision 1.2  2001/07/17 20:23:02  peter
 * *** empty log message ***
 *
 * Revision 1.1  2001/07/15 21:42:44  peter
 * dynload, time fixar mm
 *
 *
 */

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include "misc/sequence.h"

#include "os/compat.h"
#include "os/debug.h"

#include "os/gfx.h"

#include "os/exceptions.h"

#include "os/gfx_types.h"
#include "os/gfx_driver.h"
#include "os/gfxplugin.h"
#include "os/interfaces.h"

#include "gl.h"

#include <GL/glx.h>

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>


namespace reaper
{
namespace os
{
namespace lowlevel
{


using std::string;

// std::vector<video_mode> modeinfo;

using os::gfx::VideoMode;

class Gfx_sdl
 : public Gfx_driver
{
	Gfx_driver_data data;
	ifs::Gfx* main;
public:
	Gfx_sdl(ifs::Gfx* m);
	~Gfx_sdl();
	Gfx_driver_data* internal_data() { return &data; }
	void update_screen();
	bool setup_mode(const gfx::VideoMode&);
	void restore_mode();

};

Gfx_sdl::Gfx_sdl(ifs::Gfx* m)
 : main(m)
{

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw hw_fatal_error("Video initialization failed.");
	}
	atexit(SDL_Quit);

	SDL_SysWMinfo info;
	SDL_GetWMInfo(&info);
	data.display = info.info.x11.display;
//	window = info.info.x11.wmwindow;
	// FIXME
//	gfx.display = XOpenDisplay(0);

/*
	if (! (gfx.display = X11::XOpenDisplay(0))) {
		throw hw_fatal_error("Cannot open display: " +
				     string(X11::XDisplayName(0)));
	}
	gfx.screen = DefaultScreen(gfx.display);
	gfx.root = XRootWindow(gfx.display, gfx.screen);
	gfx.window = 0;
//	XVisualInfo templt;
//	int num;
//	if (! (templt.visualid =
//		XVisualIDFromVisual(DefaultVisual(gfx.display, gfx.screen))))
//		throw string("XVisualIDFromVisual");
//	gfx.visual = XGetVisualInfo(gfx.display, VisualIDMask, &templt, &num);
//	gfx.colormap = DefaultColormap(gfx.display, gfx.screen);

	info.desktop.width  = DisplayWidth(gfx.display, gfx.screen);
	info.desktop.height = DisplayHeight(gfx.display, gfx.screen);
	info.desktop.depth  = DefaultDepth(gfx.display, gfx.screen);
#ifdef XF86VM_EXT
	info.desktop.fullscreen = true;
	XF86VidModeGetAllModeLines(gfx.display, gfx.screen,
				   &gfx.num_modes, &gfx.modes);

	for (int i = 0; i < gfx.num_modes; i++) {
		VideoMode mode( gfx.modes[i]->hdisplay,
				gfx.modes[i]->vdisplay,
				info.desktop.depth, true, i);
		info.modes.insert(mode);
	}
#else
	info.desktop.fullscreen = false;
	gfx.num_modes = 0;
#endif
*/
}


bool Gfx_sdl::setup_mode(const VideoMode& mode)
{

	const SDL_VideoInfo* sdl_info = 0;
	sdl_info = SDL_GetVideoInfo();
	int bpp = sdl_info->vfmt->BitsPerPixel;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	if (SDL_SetVideoMode(mode.width, mode.height, bpp, SDL_OPENGL) == 0) {
		throw hw_fatal_error("Cannot setup video mode\n");
	}
#if 0
	derr() << mode << '\n';
	int attrib[] = { GLX_RGBA,
//		GLX_RED_SIZE, 8,
//		GLX_GREEN_SIZE, 8,
//		GLX_BLUE_SIZE, 8,
//		GLX_DEPTH_SIZE, 28,
		GLX_DOUBLEBUFFER,
		None };

	if (! (gfx.visual = glXChooseVisual(gfx.display, gfx.screen, attrib)))
		throw string("glXChooseVisual failed");
	gfx.glxcontext = glXCreateContext(gfx.display, gfx.visual, 0, 1);
	if (!gfx.glxcontext)
		throw string("glXCreateContext error");

	XSetWindowAttributes attr;
	attr.background_pixel = BlackPixel(gfx.display, gfx.screen);
	attr.colormap = XCreateColormap(gfx.display, gfx.root,
					gfx.visual->visual, AllocNone);
	unsigned long vmask = CWBackPixel | CWColormap;

	if (mode.fullscreen) {
		vmask |= CWOverrideRedirect;
		attr.override_redirect = 1;
	}
	
	gfx.window = XCreateWindow(gfx.display, gfx.root, 0, 0,
			mode.width, mode.height, 0, 
			gfx.visual->depth, InputOutput, 
			gfx.visual->visual, 
			vmask, &attr);

	long eventmask  = KeyPressMask | KeyReleaseMask | StructureNotifyMask
			| ExposureMask | PointerMotionMask 
			| ButtonPressMask | ButtonReleaseMask;

	XSelectInput(gfx.display, gfx.window, eventmask);

	Pixmap pix;
	XColor dmy;
	char data[1] = {0};
	Cursor cursor;
	pix = XCreateBitmapFromData(gfx.display, gfx.window, data, 1, 1);
	cursor = XCreatePixmapCursor(gfx.display, pix, pix, &dmy, &dmy, 0, 0);
	XFreePixmap (gfx.display, pix);

	XDefineCursor(gfx.display, gfx.window, cursor);

	glXMakeCurrent(gfx.display, gfx.window, gfx.glxcontext);
	glXWaitX();
	XMapWindow(gfx.display, gfx.window);

	if (mode.fullscreen) {
/*
		Atom motifhints = XInternAtom(gfx.display, "_MOTIF_WM_HINTS", 0);
		MotifWmHints wmhints;
		wmhints.flags = MWM_HINTS_DECORATIONS;
		wmhints.decorations = 0;
		XChangeProperty(gfx.display, gfx.window, motifhints,
				motifhints, 32, PropModeReplace,
				(unsigned char*) &wmhints, 4);
*/

#ifdef XF86VM_EXT
		XF86VidModeSwitchToMode(gfx.display, gfx.screen,
					gfx.modes[mode.id]);
		XF86VidModeSetViewPort(gfx.display, gfx.screen, 0, 0);
#endif


	}

	XGrabPointer(gfx.display, gfx.window, True, 0, 
		      GrabModeAsync, GrabModeAsync, gfx.window, 
		      None, CurrentTime);
/*
	XGrabKeyboard(gfx.display, gfx.window, True,
		      GrabModeAsync, GrabModeAsync, CurrentTime);
*/
#endif
	data.current = mode;

	const char *pr = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	data.is_accelerated = (strstr(pr, "Mesa") == 0);

	return true;
}


void Gfx_sdl::restore_mode() {
#if 0
	if (gfx.window) {
		if (info.current.fullscreen) {
			XUngrabPointer(gfx.display, CurrentTime);
			XUngrabKeyboard(gfx.display, CurrentTime);
#ifdef XF86VM_EXT
			XF86VidModeSwitchToMode(gfx.display, gfx.screen,
						gfx.modes[0]);
#endif
			glXWaitX();
		}
		XUnmapWindow(gfx.display, gfx.window);
		XDestroyWindow(gfx.display, gfx.window);
	}
	info.current = VideoMode(0,0);
#endif
}

void Gfx_sdl::update_screen()
{
#if 0
	XEvent ev;
	while (XCheckMaskEvent(gfx.display, StructureNotifyMask
					  | ExposureMask, &ev)) {
		switch (ev.type) {
		case MotionNotify: 	derr() << "MotionNotify\n";    break;
		case Expose: 		derr() << "Expose\n";	     break;
		case UnmapNotify: 	derr() << "UnmapNotify\n";     break;
		case MapNotify: 	derr() << "MapNotify\n";       break;
		case MapRequest: 	derr() << "MapRequest\n";      break;
		case ConfigureNotify: 	derr() << "ConfigureNotify\n"; break;
		case ReparentNotify: 	derr() << "ReparentNotify\n";  break;
		}
	}
#endif
	SDL_GL_SwapBuffers();
}


Gfx_sdl::~Gfx_sdl() {
	SDL_Quit();
}

}
}
}

extern "C" {

void* create_gfx_sdl(reaper::os::ifs::Gfx* m)
{
	return new reaper::os::lowlevel::Gfx_sdl(m);
}

}



