
#include "os/compat.h"

#include "os/debug.h"

#include "os/gfx_impl.h"
#include "os/event_impl.h"

#include <queue>

#include <SDL/SDL.h>
#include <SDL/SDL_keysym.h>

namespace reaper {
namespace os {
namespace lowlevel {


namespace id = event::id;
namespace { debug::DebugOutput derr("event_sdl"); }


class SDL_Device
 : public AxisDevice
{
	int x_res, y_res;

	bool key(const SDL_Event& ev);
	bool mouse(const SDL_Event& ev);
	bool joy(const SDL_Event& ev);
public:
	SDL_Device() {
		SDL_Surface* s = SDL_GetVideoSurface();
		x_res = s->w;
		y_res = s->h;
	}
	bool poll_device() {
		SDL_Event event;
		if (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				return key(event);
				break;
			case SDL_MOUSEMOTION:
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				return mouse(event);
				break;
			case SDL_JOYAXISMOTION:
			case SDL_JOYBALLMOTION:
			case SDL_JOYHATMOTION:
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				return joy(event);
				break;
			default:
				break;
			}
			return false;
		}
		return false;
	}
};


id::EventID translate_key(SDL_keysym key) {
	switch (key.sym) {
	case SDLK_RETURN:	return id::Enter;
	case SDLK_DOWN:		return id::Down;
	case SDLK_UP:		return id::Up;
	case SDLK_LEFT:		return id::Left;
	case SDLK_RIGHT:	return id::Right;
	case SDLK_ESCAPE:	return id::Escape;
	case SDLK_PAGEUP:  	return id::PageUp;
	case SDLK_PAGEDOWN:   	return id::PageDown;
	case SDLK_DELETE: 	return id::Delete;
	case SDLK_INSERT: 	return id::Insert;
	case SDLK_BACKSPACE: 	return id::Backspace;
	case SDLK_F1:		return id::F1;
	case SDLK_F2:		return id::F2;
	case SDLK_F3:		return id::F3;
	case SDLK_F4:		return id::F4;
	case SDLK_F5:		return id::F5;
	case SDLK_F6:		return id::F6;
	case SDLK_F7:		return id::F7;
	case SDLK_F8:		return id::F8;
	case SDLK_F9:		return id::F9;
	case SDLK_F10:		return id::F10;
	case SDLK_F11:		return id::F11;
	case SDLK_F12:		return id::F12;
	}
	if (key.sym < 256)
		return id::mk(toupper(key.sym));
	else
		return id::Unknown;
}


bool SDL_Device::key(const SDL_Event& ev)
{
	const SDL_KeyboardEvent& kev = ev.key;
	pending.push((*map)(Event(translate_key(kev.keysym), kev.state == SDL_PRESSED)));
	return true;
}

int sdl_btn(int b) {
	switch (b) {
	case SDL_BUTTON_LEFT: return 0;
	case SDL_BUTTON_RIGHT: return 1;
	case SDL_BUTTON_MIDDLE: return 2;
	}
	return 3;
}

bool SDL_Device::mouse(const SDL_Event& ev)
{
	if (ev.type == SDL_MOUSEMOTION) {
		const SDL_MouseMotionEvent& mev = ev.motion;
		float x_r = norm(mev.x, x_res);
		float y_r = norm(mev.y, y_res);
		pending.push((*map)(Event(0, x_r)));
		pending.push((*map)(Event(1, y_r)));
	} else {
		const SDL_MouseButtonEvent& mev = ev.button;
		pending.push((*map)(Event(sdl_btn(mev.button), mev.state == SDL_PRESSED)));
	}
	return true;
}


bool SDL_Device::joy(const SDL_Event& ev)
{
	if (ev.type == SDL_JOYAXISMOTION) {
		const SDL_JoyAxisEvent& jev = ev.jaxis;
		pending.push((*map)(Event(2 + jev.axis, norm(jev.value + 32768, config[jev.axis]))));
		return true;
	}
	return false;
}



class IDMSDL
 : public InputDeviceModule
{
	void IDMSDL::scan_inputdevices(Gfx_driver_data* g,
		device_inserter out)
	{
		*out++ = new SDL_Device();
	}
};

}
}
}

extern "C" {
DLL_EXPORT void* create_event_sdl()
{
	return new reaper::os::lowlevel::IDMSDL();
}
}

