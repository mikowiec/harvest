/* $Id: event_win32.cpp,v 1.5 2003/03/28 15:40:03 pstrand Exp $ */


#include "os/compat.h"

#include <queue>

#include "misc/plugin.h"

#include "os/debug.h"
#include "os/gfx/gfx_types.h"
#include "os/event/event_impl.h"
#include "os/interfaces.h"
#include "os/win32_errors.h"

#include "os/dxwrap.h"

namespace reaper {
namespace os {
namespace event {

using lowlevel::Gfx_driver_data;

class Keyboard : public InputDevice {

	event::id::EventID translate_key(unsigned key) {
		using namespace event::id;
		switch (key) {
		case VK_DOWN:	   return Down;
		case VK_UP:	   return Up;
		case VK_LEFT:	   return Left;
		case VK_RIGHT:	   return Right;
		case VK_ESCAPE:	   return Escape;
		case VK_RETURN:    return Enter;
		case VK_INSERT:    return Insert;
		case VK_DELETE:    return Delete;
		case VK_NEXT:      return PageDown;
		case VK_PRIOR:     return PageUp;
		case VK_HOME:	   return Home;
		case VK_END:	   return End;
		case VK_BACK:	   return Backspace;
		case VK_F1:	   return F1;
		case VK_F2:	   return F2;
		case VK_F3:	   return F3;
		case VK_F4:	   return F4;
		case VK_F5:	   return F5;
		case VK_F6:	   return F6;
		case VK_F7:	   return F7;
		case VK_F8:	   return F8;
		case VK_F9:	   return F9;
		case VK_F10:	   return F10;
		case VK_F11:	   return F11;
		case VK_F12:	   return F12;
		}
		if (key < 256)
			return static_cast<EventID>(key);
		return Unknown;
	}
	Gfx_driver_data* gx;
protected:
	bool Keyboard::poll_device()
	{
		if (gx->key_queue.empty()) {
			return false;
		}
		std::pair<int,bool> p = gx->key_queue.front();
		pending.push(Event(translate_key(p.first), p.second));
		gx->key_queue.pop();
		return true;
	}
public:
	Keyboard(Gfx_driver_data* g, ifs::Event* m) : gx(g)
	{ }
};


class Mouse : public InputDevice {
	float cur_x, cur_y;
	di_ptr di;
	di_mouse dev;
//	LPDIRECTINPUTDEVICE di_mouse;
	const Gfx_driver_data* gx;
protected:
	bool poll_device();
	void init();
	ifs::Event* main;
public:
	Mouse(const Gfx_driver_data* g, ifs::Event* ep) : cur_x(0.0), cur_y(0.0), gx(g), main(ep) {
		init();
	}
	~Mouse() {
		di_mouse_drop(dev);
		di_mouse_release(dev);
		di_release(di);
	}
};


void Mouse::init()
{
	di = di_get();
	bool nograb = main->config("nograb") == "true";
	dev = di_init_mouse(di, !nograb);
	if (!dev)
		throw di_last_error(di);
/*
	int err;
	di_ptr di = dinput().ptr();

	di_mouse = 0;
	if ((err = di->CreateDevice(GUID_SysMouse, &di_mouse, 0)) != DI_OK)
		throw input_error(std::string("CreateDevice failed: ") + di_error(err));

	if ((err = di_mouse->SetCooperativeLevel(get_hwnd(), DISCL_EXCLUSIVE | DISCL_FOREGROUND)) != DI_OK)
		throw input_error(std::string("SetCooperativeLevel failed: ") + di_error(err));

	if ((err = di_mouse->SetDataFormat(&c_dfDIMouse)) != DI_OK)
		throw input_error(std::string("SetDataFormat failed: ") + di_error(err));
*/
}

bool range_ok(float val, float ch)
{
	return (val > -1.0 && ch < 0) || (val < 1.0 && ch > 0);
}

bool Mouse::poll_device()
{
	if (!gx->window_active) {
		di_mouse_drop(dev);
		return false;
	}
//	DIMOUSESTATE state;
	di_mousestate state;
	int err = 1, tries = 0;
	do {
		/*
		err = di_mouse->GetDeviceState(sizeof(state), &state);
		if (err == DIERR_INPUTLOST || err == DIERR_NOTACQUIRED) {
//			main->derr() << "(mouse) input lost or unacq\n";
			di_mouse->Acquire();
		}
		*/
		err = di_read_mouse(dev, &state);
		if (tries++ > 2) {
//			main->derr() << "nah...\n";
			return false;
		}
	} while (err);

	const int x_res = gx->current.width;
	const int y_res = gx->current.height;

	if (state.lX != 0 && range_ok(cur_x, state.lX)) {
		cur_x += (float)state.lX*3 / x_res;
		pending.push(Event(0, cur_x));
	}
	if (state.lY != 0 && range_ok(cur_x, state.lY)) {
		cur_y += (float)state.lY*3 / y_res;
		pending.push(Event(1, cur_y));
	}

	static bool btnstate[4];
	for (int i = 0; i < 3; i++) {
		bool st = (state.rgbButtons[i] & 0x80) == 0;
		if (btnstate[i] != st) {
			pending.push(Event(i, !st));
			btnstate[i] = st;
		}
	}
	return !pending.empty();
}


class Joystick : public InputDevice {
protected:
	ifs::Event* ep;
	enum { x_axis = 2, y_axis = 3 };
	std::deque<int> axis_val;
	void read_axis(int, int);
	virtual bool real_init() = 0;
public:
	Joystick(ifs::Event* e)
	 : ep(e)
	{
		axis_val.resize(2);
		axis_val[0] = 0;
		axis_val[1] = 0;
	}
	bool init()
	{
		return real_init();
	}
};


class StdJoystick : public Joystick
{
protected:
	bool poll_device();
	bool real_init();
public:
	StdJoystick(ifs::Event* e) : Joystick(e) { }
};

std::string joyerr(int e) 
{
	switch (e) {
	case MMSYSERR_NODRIVER:    return "No driver";
	case MMSYSERR_INVALPARAM:  return "Invalid parameter";
	case MMSYSERR_BADDEVICEID: return "Bad deviceid";
	case JOYERR_UNPLUGGED:     return "No joystick found";
	default:		   return "unknown error";
	}
}

bool StdJoystick::real_init()
{
	JOYINFOEX state;
	state.dwSize = sizeof(state);
	state.dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS;
	return joyGetPosEx(JOYSTICKID1, &state) == JOYERR_NOERROR;
}


void Joystick::read_axis(int a, int new_val)
{
	if (has_changed(ep->axis_config(a), axis_val[a], new_val)) {
		axis_val[a] = new_val;
		float v = norm(axis_val[a], ep->axis_config(a));
		pending.push(Event(a, v));
//		if (debug)
//			derr << a << " axis " << axis_val[a] << " -> " << v << '\n';
	}
}



bool StdJoystick::poll_device()
{
	JOYINFOEX state;
	state.dwSize = sizeof(state);
	state.dwFlags = JOY_RETURNX | JOY_RETURNY | JOY_RETURNBUTTONS;
	int err;
	if ((err = joyGetPosEx(JOYSTICKID1, &state)) != JOYERR_NOERROR) {
		throw input_error(std::string("Joystick disabled: ") + joyerr(err));
	}

	static bool btnstate[32];

	read_axis(x_axis, state.dwXpos);
	read_axis(y_axis, state.dwYpos);

	for (int i = 0; i < 4; i++) {
		bool st = (state.dwButtons & (1 << i)) != 0;
		if (btnstate[i] != st) {
			btnstate[i] = st;
			pending.push(Event((3 + i), st));
//			if (debug)
//				derr << "Button " << i << ' ' << st << '\n';
		}
	}
	return !pending.empty();
}




class DInputJoystick : public Joystick
{
//	LPDIRECTINPUTDEVICE di_joy;
	di_ptr di;
	di_joy dev;
protected:
	bool poll_device();
	bool real_init()
	{
		di = di_get();
		dev = di_init_joystick(di);
		return di && dev;
	}
public:
	DInputJoystick(ifs::Event* e) : Joystick(e) { }
	~DInputJoystick()
	{
		di_release(di);
	}
};

/*
typedef std::pair<di_ptr,LPDIRECTINPUTDEVICE> joypair;

// Yuck!
BOOL FAR PASCAL test_joy(LPCDIDEVICEINSTANCE pdinst, void* v)
{
	joypair* dp = static_cast<joypair*>(v);

	if ((dp->first->CreateDevice(pdinst->guidInstance, &dp->second, 0) ==  DI_OK)
	 && (dp->second->SetDataFormat(&c_dfDIJoystick) == DI_OK)
	 && (dp->second->SetCooperativeLevel(get_hwnd(), DISCL_EXCLUSIVE | DISCL_FOREGROUND) == DI_OK)
	 && (dp->second->Acquire() == DI_OK))
		return DIENUM_STOP;
	dp->second = 0;
	return DIENUM_CONTINUE;
}


bool DInputJoystick::real_init() {
	int err;
	joypair jp(dinput().ptr(), 0);
	err = jp.first->EnumDevices(DIDEVTYPE_JOYSTICK, test_joy, &jp, DIEDFL_ATTACHEDONLY);
	di_joy = (err == DI_OK) ? jp.second : 0;
	return err == DI_OK && jp.second;
}
*/


bool DInputJoystick::poll_device()
{
	di_joystate state;
//	DIJOYSTATE state;
//	DIJOYSTATE2 state;

	int err;
//	err = di_joy->GetDeviceState(sizeof(state), &state);
	err = di_read_joystick(dev, &state);
	if (err)
		throw input_error(di_error(err));

	static int x_pos = 0;
	static int y_pos = 0;
	static bool btnstate[4] = {false, false, false, false};

	read_axis(x_axis, state.lX);
	read_axis(y_axis, state.lY);

	for (int i = 0; i < 4; i++) {
		bool st = (state.rgbButtons[i] & 0x80) != 0;
		if (btnstate[i] != st) {
			btnstate[i] = st;
			pending.push(Event((3 + i), st));
//			if (debug)
//				derr << "Button " << i << ' ' << st << '\n';
		}
	}
	return !pending.empty();  
}

class IDMWin32
 : public InputDeviceModule
{
	ifs::Event* e;
public:
	IDMWin32(ifs::Event* ep) : e(ep) { }
	void scan_inputdevices(Gfx_driver_data* g, std::back_insert_iterator<InputDevices> out)
	{
		*out++ = new Keyboard(g, e);
		*out++ = new Mouse(g, e);
		Joystick* js = new DInputJoystick(e);
		if (js->init())
			*out++ = js;
		else {
			delete js;
			js = new StdJoystick(e);
			if (js->init())
				*out++ = js;
		}
	}
};


}
}
}


extern "C" {
DLL_EXPORT void* create_event_win32(reaper::os::ifs::Event* e)
{
	return new reaper::os::event::IDMWin32(e);
}
}


#ifdef REAPER_EMBED_PLUGINS
namespace {
struct RegisterPlugin {
	RegisterPlugin() {
		reaper::misc::preloaded::add_plugin_creator("event_win32", (reaper::misc::VoidCreator1)create_event_win32);
	}
} foo;
}
#endif

