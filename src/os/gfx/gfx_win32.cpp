/*
 * $Author: pstrand $
 * $Date: 2003/03/28 15:40:36 $
 * $Log: gfx_win32.cpp,v $
 * Revision 1.7  2003/03/28 15:40:36  pstrand
 * mouse grabbing
 *
 * Revision 1.6  2003/03/28 13:48:58  pstrand
 * better handling of resize
 *
 * Revision 1.5  2003/03/25 02:19:13  pstrand
 * *** empty log message ***
 *
 * Revision 1.4  2003/03/19 22:09:49  pstrand
 * support embedded plugins
 *
 * Revision 1.3  2003/01/06 23:04:57  pstrand
 * hw->os
 *
 * Revision 1.2  2003/01/06 12:42:13  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.73  2002/06/06 07:57:56  pstrand
 * no gfx/ dep, glh_init fix
 *
 * Revision 1.72  2002/05/21 03:07:51  fizzgig
 * envmap update options (gfx_renderer::environment_mapping: 0->off 1->incremental 2->instant)
 * preliminary support for shadow volumes (now requests stencil buffer as well)
 *
 * Revision 1.71  2002/03/12 02:22:21  pstrand
 * *** empty log message ***
 *
 * Revision 1.70  2002/03/11 10:50:48  pstrand
 * hw/time.h -> reltime.h&abstime.h, and some more..
 *
 * Revision 1.69  2002/02/13 14:17:47  peter
 * alpha info..
 *
 * Revision 1.68  2002/02/08 11:27:54  macke
 * rendering reorg and silhouette shadows!
 *
 * Revision 1.67  2002/01/03 22:34:13  peter
 * resursövergång
 *
 * Revision 1.66  2001/12/24 14:40:26  peter
 * 0.93
 *
 * Revision 1.65  2001/11/26 19:35:51  peter
 * no message
 *
 * Revision 1.64  2001/10/10 01:23:47  peter
 * mindre windows-include...
 *
 * Revision 1.63  2001/10/09 23:15:45  peter
 * no message
 *
 * Revision 1.62  2001/09/24 21:13:12  macke
 * Korrekt viewport vid fullscreen...
 *
 * Revision 1.61  2001/08/09 22:44:51  peter
 * ...
 *
 * Revision 1.60  2001/08/09 22:43:41  peter
 * resize
 *
 * Revision 1.59  2001/08/06 12:16:21  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.58.2.1  2001/07/31 17:32:09  peter
 * testgren...
 *
 *
 */


#include "os/compat.h"

#include "os/windows.h"

#include <string>
#include <fstream>
#include <queue>
#include <iostream>
#include <set>

#include "os/gfx.h"
#include "os/gfx/gfx_types.h"
#include "os/gfx/gfx_driver.h"

#include "os/interfaces.h"
#include "os/exceptions.h"

#include "os/event/event_impl.h"
#include "os/debug.h"
#include "os/exceptions.h"

#include "misc/sequence.h"
#include "misc/parse.h"
#include "misc/plugin.h"
#include "misc/prim_iter_impl.h"

#include "os/gl.h"

namespace
{
	float real_width, real_height;
}

namespace reaper
{
namespace os
{
namespace gfx
{


} // end gfx namespace

namespace lowlevel
{

typedef DEVMODE video_mode;


class Gfx_win32
 : public Gfx_driver
{
	bool valid;
	bool di_valid;
	bool reinit;
	HGLRC glrc;
	HPALETTE palette;
	HINSTANCE inst;
	int cmdshow;
	int depth;

	ifs::Gfx* main;

	std::set<gfx::VideoMode> vmodes;

public:
	void fix_wnd(int width, int height);

	HWND window;
	Gfx_driver_data data;
	Gfx_win32(ifs::Gfx* m);
	~Gfx_win32();
	Gfx_driver_data* internal_data() { return &data; }
	bool update_screen();
	bool setup_mode(const gfx::VideoMode&);
	void restore_mode();

	misc::prim_iter_if<gfx::VideoMode>* modes() const;
};

Gfx_win32* me;


LRESULT
__stdcall msghandler(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
//	derr << "msghandler" << wnd << " " << msg << " " << wp << " " << lp << '\n';
	switch (msg) {
	case WM_CREATE:
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(wnd, &ps);
		EndPaint(wnd, &ps);
		break;
	case WM_SIZE:
		me->fix_wnd(LOWORD(lp), HIWORD(lp));
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		me->data.key_queue.push(std::make_pair(wp, true));
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		me->data.key_queue.push(std::make_pair(wp, false));
		break;
    	case WM_MOUSEMOVE:
//		mouse_queue.push(std::make_pair(LOWORD(lp), HIWORD(lp)));
		break;
	case WM_LBUTTONDOWN:
//		btn_queue.push(std::make_pair(0, true));
		break;
	case WM_LBUTTONUP:
//		btn_queue.push(std::make_pair(0, false));
		break;
	case WM_RBUTTONDOWN:
//		btn_queue.push(std::make_pair(1, true));
		break;
	case WM_RBUTTONUP:
//		btn_queue.push(std::make_pair(1, false));
		break;
	case WM_EXITSIZEMOVE:
	case WM_EXITMENULOOP:
	case WM_SETFOCUS:
//		printf("focus\n");
		me->data.window_active = true;
		break;
	case WM_ENTERSIZEMOVE:
	case WM_ENTERMENULOOP:
	case WM_KILLFOCUS:
//		printf("unfocus\n");
		me->data.window_active = false;
		break;
	default:
		break;
	}
	return DefWindowProc(wnd, msg, wp, lp);
}



char* cname = "Reaper";
char* wname = "Reaper";

using gfx::VideoMode;

std::vector<video_mode> modeinfo;

Gfx_win32::Gfx_win32(ifs::Gfx* m) : main(m) {
	valid = false;
	di_valid = false;
	reinit = false;
	WNDCLASS wclass;
	inst = GetModuleHandle(0);

	wclass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wclass.lpfnWndProc = msghandler;
	wclass.cbClsExtra = 0;
	wclass.cbWndExtra = 0;
	wclass.hInstance = inst;
	wclass.hIcon = LoadIcon(0, IDI_APPLICATION);
	wclass.hCursor = LoadCursor(0, IDC_ARROW);
	wclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wclass.lpszMenuName = 0;
	wclass.lpszClassName = cname;
	if (RegisterClass(&wclass) == 0) {
		throw hw_fatal_error("RegisterClass failed");
	}


	video_mode mode;
	EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &mode);
	data.desktop = VideoMode(mode.dmPelsWidth,
				 mode.dmPelsHeight,
				 true);
	int more, i = 0;
	do {
		more = EnumDisplaySettings(0, i, &mode);
		if (mode.dmPelsHeight >= 480) {
			gfx::VideoMode vm(mode.dmPelsWidth,
					  mode.dmPelsHeight,
					  true);
			vmodes.insert(vm);
			modeinfo.push_back(mode);
		}
		i++;
	} while (more);
	me = this;
	data.window_active = false;
}

Gfx_win32::~Gfx_win32() {
	restore_mode();
	UnregisterClass(cname, GetModuleHandle(0));
}

bool test_hw(PIXELFORMATDESCRIPTOR& pfd, HDC hDC)
{
	int generic_format = pfd.dwFlags & PFD_GENERIC_FORMAT;
	int generic_accelerated = pfd.dwFlags & PFD_GENERIC_ACCELERATED;

	if (generic_format && ! generic_accelerated) {
//		printf("software\n");
		return false;
	} else if (generic_format && generic_accelerated) {
//		printf("hardware - MCD\n");
		return true;
	} else if (! generic_format && ! generic_accelerated) {
//		printf("hardware - ICD\n");
		return true;
	}
	return false;
}


bool Gfx_win32::setup_mode(const VideoMode& mode)
{

	bool fullscreen = mode.fullscreen;

	if (fullscreen) {		
		DEVMODE dmScreenSettings;					// Device Mode

		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));		// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth  = mode.width;			// Selected Screen Width
		dmScreenSettings.dmPelsHeight = mode.height;			// Selected Screen Height
//		dmScreenSettings.dmBitsPerPel = mode.depth;			// Selected Bits Per Pixel

		dmScreenSettings.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN)
		    != DISP_CHANGE_SUCCESSFUL) {
//			derr << "Change to full screen failed. Reverting to windowed mode\n";
			fullscreen = false;
		}
	}
	int style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
	if (fullscreen)
		style |= WS_POPUP;
	else
		style |= WS_OVERLAPPEDWINDOW;


	int pos_x = 0; // conf["pos_x"];
	int pos_y = 0; // conf["pos_y"];

	int w = mode.width;
	int h = mode.height;

	if(!fullscreen) {
		w += GetSystemMetrics(SM_CXFRAME);
		h += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);
	}

	window = CreateWindow(cname, wname, style, pos_x, pos_y, w, h, 0, 0, inst, 0);

	if (! window)
		throw hw_fatal_error(std::string("CreateWindow failed: ")
			+ misc::ltos(GetLastError()));
	data.current = mode;
//	if (data.current.depth == 0)
//		data.current.depth = depth;

	HDC dc = GetDC(window);
	if (! dc)
		throw hw_fatal_error("GetDC failed");

	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL 
		    | PFD_TYPE_RGBA | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pf_data[][4] = { 
		{32, 24, 8, 8}, 
		{24, 24, 8, 8}, 
		{16, 24, 8, 8}, 
		{16, 16, 8, 8}, 

		{32, 24, 8, 0}, 
		{24, 24, 8, 0}, 
		{16, 24, 8, 0}, 
		{16, 16, 8, 0}, 

		{32, 24, 0, 8}, 
		{24, 24, 0, 8}, 
		{16, 24, 0, 8},
		{16, 16, 0, 8}, 

		{32, 24, 0, 0}, 
		{24, 24, 0, 0}, 
		{16, 24, 0, 0}, 
		{16, 16, 0, 0}, 
		{-1,-1,-1, -1} 
	};

	int pixel_format = 0;
	for (int i = 0; pf_data[i][0] != -1; ++i) {
		pfd.cDepthBits = pf_data[i][0];
		pfd.cColorBits = pf_data[i][1];
		pfd.cAlphaBits = pf_data[i][2];
		pfd.cStencilBits = pf_data[i][3];

		pixel_format = ChoosePixelFormat(dc, &pfd);
		if (! pixel_format)
			continue;
		PIXELFORMATDESCRIPTOR pfd_tmp;
		DescribePixelFormat(dc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pfd_tmp);
		if (test_hw(pfd_tmp, dc))
			break;
	}
	if (((int)pfd.cDepthBits) < 0)
		throw hw_fatal_error("no suitable pixelformat found!");
	data.is_accelerated = test_hw(pfd, dc);
	data.alpha = pfd.cAlphaBits != 0;
	data.stencil = pfd.cStencilBits != 0;

	if (!SetPixelFormat(dc, pixel_format, &pfd))
		throw hw_fatal_error("SetPixelFormat failed");

	if (! (glrc = wglCreateContext(dc)) )
		throw hw_fatal_error("wglCreateContext failed");
	if (! wglMakeCurrent(dc, glrc))
		throw hw_fatal_error("wglMakeCurrent failed");

	ReleaseDC(window, dc);

	ShowWindow(window, SW_SHOW);

	reinit = true;
	valid = true;
	return true;
}

void Gfx_win32::fix_wnd(int w, int h)
{
	data.current.width = w;
	data.current.height = h;
}

void Gfx_win32::restore_mode() {
	if (valid) {
		DestroyWindow(window);
		ChangeDisplaySettings(0, 0);
		valid = false;
	}
	if (di_valid) {
		di_valid = false;
	}
}


void process_input(Gfx_win32* gx) {
	MSG msg;
	while (PeekMessage(&msg, gx->window, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}


bool Gfx_win32::update_screen() {

	HDC dc = GetDC(window);
	SwapBuffers(dc);
	ReleaseDC(window, dc);
	process_input(this);
	return false;
}

misc::prim_iter_if<VideoMode>* Gfx_win32::modes() const
{
	return misc::mk_iter(vmodes);
}


}
}
}

extern "C" {
DLL_EXPORT void* create_gfx_win32(reaper::os::ifs::Gfx* m)
{
	return new reaper::os::lowlevel::Gfx_win32(m);
}
}

#ifdef REAPER_EMBED_PLUGINS
namespace {
struct RegisterPlugin {
	RegisterPlugin() {
		reaper::misc::preloaded::add_plugin_creator("gfx_win32", (reaper::misc::VoidCreator1)create_gfx_win32);
	}
} foo;
}
#endif

