
#include "os/compat.h"

#define DIRECTINPUT_VERSION 0x0500

#include <dinput.h>

#include "os/dxwrap.h"
#include "os/win32_errors.h"

#include <stdio.h>

HWND get_hwnd()
{
	return GetForegroundWindow();
}


struct dinput {
	LPDIRECTINPUT ptr;
	char* err;
};


di_ptr di_mgr(int w)
{
	static struct dinput di = {0, 0};
	static int count = 0;
	int err;
	if (w) {
		di.err = (char*)calloc(1, 100);
		if ((err = DirectInputCreate(GetModuleHandle(0), DIRECTINPUT_VERSION, &di.ptr, 0)) != DI_OK) {
			if ((err = DirectInputCreate(GetModuleHandle(0), 0x0300, &di.ptr, 0)) != DI_OK) {
				_snprintf(di.err, 100, "error creating dinput");
				return 0;
			}
		}
		count++;
		return &di;
	} else {
		if (--count == 0) {
			di.ptr->lpVtbl->Release(di.ptr);
		}
		return 0;
	}
}

di_ptr di_get()
{
	return di_mgr(1);
}

void di_release(di_ptr p)
{
	di_mgr(0);
}

const char* di_last_error(di_ptr p)
{
	return p->err;
}

di_mouse di_init_mouse(di_ptr di, int grab)
{
	int err, mode;
	DIDATAFORMAT fmt = c_dfDIMouse;
	LPDIRECTINPUTDEVICE dm;
	if ((err = di->ptr->lpVtbl->CreateDevice(di->ptr, &GUID_SysMouse, &dm, 0)) != DI_OK) {
		_snprintf(di->err, 100, "CreateDevice failed: %s", di_error(err));
		return 0;
	}
	ShowCursor(FALSE);
	mode = DISCL_FOREGROUND;
	if (grab)
		mode |= DISCL_EXCLUSIVE;
	else 
		mode |= DISCL_NONEXCLUSIVE;
	if ((err = dm->lpVtbl->SetCooperativeLevel(dm, get_hwnd(), mode)) != DI_OK) {
		_snprintf(di->err, 100, "SetCooperativeLevel failed: %s", di_error(err));
		return 0;
	}
//	fmt.dwFlags = DIDF_ABSAXIS;
	if ((err = dm->lpVtbl->SetDataFormat(dm, &fmt)) != DI_OK) {
		_snprintf(di->err, 100, "SetDataFormat failed: %s", di_error(err));
		return 0;
	}
	return dm;
}

int di_read_mouse(di_mouse dm, di_mousestate *state)
{
	int err = dm->lpVtbl->GetDeviceState(dm, sizeof(di_mousestate), state);
	if (err == DIERR_INPUTLOST || err == DIERR_NOTACQUIRED) {
//			main->derr() << "(mouse) input lost or unacq\n";
		dm->lpVtbl->Acquire(dm);
		return 1;
	}
	return 0;
}

void di_mouse_drop(di_mouse dm)
{
	dm->lpVtbl->Unacquire(dm);
}

void di_mouse_release(di_mouse dm)
{
	dm->lpVtbl->Release(dm);
}

typedef struct {
	di_ptr di;
	LPDIRECTINPUTDEVICE dev;
} joypair;

// Yuck!
BOOL FAR PASCAL test_joy(LPCDIDEVICEINSTANCE pdinst, void* v)
{
	joypair* dp = (joypair*)v;

	if ((dp->di->ptr->lpVtbl->CreateDevice(dp->di->ptr, &pdinst->guidInstance, &dp->dev, 0) ==  DI_OK)
	 && (dp->dev->lpVtbl->SetDataFormat(dp->dev, &c_dfDIJoystick) == DI_OK)
	 && (dp->dev->lpVtbl->SetCooperativeLevel(dp->dev, get_hwnd(), DISCL_EXCLUSIVE | DISCL_FOREGROUND) == DI_OK)
	 && (dp->dev->lpVtbl->Acquire(dp->dev) == DI_OK))
		return DIENUM_STOP;
	dp->dev= 0;
	return DIENUM_CONTINUE;
}




di_joy di_init_joystick(di_ptr di)
{
	int err;
	joypair jp = {di, 0};
	err = jp.di->ptr->lpVtbl->EnumDevices(jp.di->ptr, DIDEVTYPE_JOYSTICK, test_joy, &jp, DIEDFL_ATTACHEDONLY);
	return (!err) ? jp.dev : 0;
}


int di_read_joystick(di_joy dj, di_joystate *state)
{
	int err = dj->lpVtbl->GetDeviceState(dj, sizeof(di_joystate), state);
	return err;
}



