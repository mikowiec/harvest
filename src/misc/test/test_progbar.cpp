#include "os/compat.h"

#include <stdio.h>

#include "os/gl.h"

#include "os/gfx.h"
#include "os/time.h"
#include "misc/test_main.h"

#include "game/helpers.h"

using namespace reaper;


debug::DebugOutput derr("test_progbar",0);


int test_main()
{
	os::gfx::Gfx gx;
	game::ProgressBar bar(gx, "Loading", 4);

	bar.render("test");
	os::time::msleep(1000);
	bar.tick("one");
	os::time::msleep(1000);
	bar.tick("two");
	os::time::msleep(1000);
	bar.tick("three");
	os::time::msleep(1000);
	bar.tick("five");
	os::time::msleep(1000);

	return 0;
}


