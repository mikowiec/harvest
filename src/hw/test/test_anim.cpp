
#include "os/compat.h"

#include "misc/test_main.h"
#include "os/video.h"
#include "os/gfx.h"
#include "os/gl.h"

#include <fstream>

using namespace reaper;


int test_main()
{
	os::gfx::Gfx gx;
	os::video::Player* player = new os::video::Player(gx, "mel");

	player->initgfx();
	player->play();
	return 0;
}
 

