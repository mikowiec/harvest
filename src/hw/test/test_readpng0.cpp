
#include "os/compat.h"

#include "misc/test_main.h"
#include "os/gfx.h"
#include "gfx/texture.h"

using namespace reaper;

int test_main()
{
	os::gfx::Gfx gx;
	gfx::texture::Texture("main");
	return 0;
}

