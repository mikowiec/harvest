#ifndef REAPER_HW_DXWRAP_H
#define REAPER_HW_DXWRAP_H

#include "os/windows.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

struct dinput;
typedef struct dinput* di_ptr;

di_ptr di_get();
void di_release(di_ptr);
const char* di_last_error(di_ptr);



typedef struct {
	LONG lX;
	LONG lY;
	LONG lZ;
	BYTE rgbButtons[4];
} di_mousestate;

struct IDirectInputDevice;
typedef struct IDirectInputDevice *di_mouse;

di_mouse di_init_mouse(di_ptr, int grab);
int di_read_mouse(di_mouse, di_mousestate*);
void di_mouse_drop(di_mouse);
void di_mouse_release(di_mouse);

typedef struct {
	LONG    lX;
	LONG    lY;
	LONG    lZ;
	LONG    lRx;
	LONG    lRy;
	LONG    lRz;
	LONG    rglSlider[2];
	DWORD   rgdwPOV[4];
	BYTE    rgbButtons[32];
} di_joystate;


typedef struct IDirectInputDevice *di_joy;

di_joy di_init_joystick(di_ptr);
int di_read_joystick(di_joy, di_joystate*);


#ifdef __cplusplus
}
#endif

#endif

