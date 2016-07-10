
#include <windows.h>
#include <dinput.h>
#include <stdio.h>

#pragma comment(lib, "dinput.lib")
#pragma comment(lib, "dxguid.lib")


BOOL FAR PASCAL test_joy(LPCDIDEVICEINSTANCE pdinst, void* v)
{
	LPDIRECTINPUT dinput = static_cast<LPDIRECTINPUT>(v);
	LPDIRECTINPUTDEVICE di_joystick;
	printf("Found: %s %s\n", pdinst->tszInstanceName, pdinst->tszProductName);
	if (dinput->CreateDevice(pdinst->guidInstance, &di_joystick, 0) !=  DI_OK)
		return DIENUM_CONTINUE;
	printf("Success 1\n");
	if (di_joystick->SetDataFormat(&c_dfDIJoystick) != DI_OK)
		return DIENUM_CONTINUE;
	printf("Success 2\n");
	if (di_joystick->Acquire() != DI_OK)
		return DIENUM_CONTINUE;
	printf("Success 3\n");
	return DIENUM_CONTINUE;
}

int main()
{
	LPDIRECTINPUT dinput;
	if (DirectInputCreate(GetModuleHandle(0), 0x0500, &dinput, 0) == DI_OK)
		dinput->EnumDevices(0, test_joy, dinput, DIEDFL_ALLDEVICES);

	return 0;
}
