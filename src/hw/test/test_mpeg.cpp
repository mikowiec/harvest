
#include "os/compat.h"

#include "os/debug.h"
#include "os/gfx.h"
#include "os/snd.h"
#include "os/time.h"

#include "misc/test_main.h"

#include <iostream>

using namespace reaper;

int test_main()
{
//	os::gfx::Gfx gx;
//	gx->change_mode(os::gfx::VideoMode(640, 480));
	os::snd::SoundSystem ss;

	os::snd::Music* mp3 = ss.prepare_music("test");

	mp3->play();

	os::time::TimeSpan now = os::time::get_time();
	while (! exit_now()) {
		os::time::msleep(100);
//			ss.do_stuff();
	}
}


