#include "os/compat.h"
#include "os/gfx.h"
#include "misc/test_main.h"
#include "game/helpers.h"

using namespace reaper;
using namespace object;

int foo()
{
	debug::debug_disable("world");

	os::gfx::Gfx gx;

	game::Game* go = new game::Game(gx);

	go->game_init();
	go->game_start();

	bool quit = false;

	while (!quit && !exit_now()) {
		quit = go->loop_once();
	}
	go->shutdown();

	derr << "Successful exit. Average FPS = "
	     << 1000000.0 / go->timers.frame.get_avg() << '\n';
	delete go;

	return 0;
}

int test_main()
{
	int r = 0;
	char c = 'n';
	do {
		r = foo();	
//		cout << "More?\n";
//		cin >> c;
	} while (c == 'y');
	return r;
}
 
