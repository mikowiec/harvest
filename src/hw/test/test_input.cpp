
#include "os/compat.h"

#include <iostream>
#include <sstream>
#include <map>

#include "os/gfx.h"
#include "os/event.h"
#include "os/time.h"
#include "os/debug.h"
#include "os/worker.h"
#include "net/server.h"
#include "misc/test_main.h"
#include "res/config.h"
#include "object/base.h"
#include "object/object_impl.h"
#include "main/types.h"
#include "ai/msg.h"
#include "gfx/misc.h"
#include "res/res.h"

#include "game/helpers.h"


using namespace reaper;
using namespace object;

namespace reaper {
namespace misc {
	void print_counts();
}
namespace object {
	int get_obj_count();
}
}

class AutoControls
{
	controls::Ship& ctrl;
public:
	AutoControls(controls::Ship& c)
	 : ctrl(c) { }
	void think();
};


class ServerJob
 : public os::worker::Job
{
	net::GameServer gs;
public:
	bool operator()() {
		gs.run();
		return false;
	}
};

int foo()
{
	os::time::Ticker perf;

	os::gfx::Gfx gx;

	std::auto_ptr<game::Game> go(new game::Game(gx));

	if (args.size() > 0 && args[0] == "obs")
		go->conf.observer = true;

	if (args.size() > 0 && args[0] == "srv") {
		go->conf.run_multi = true;
		os::worker::worker()->spawn_job(new ServerJob());
	}

	go->game_init();
	go->game_start();

	int quit = 0;

	while (!quit && !exit_now()) {
		quit = go->loop_once();
	}

	derr << "Successful exit. Average FPS = "
	     << 1000000.0 / go->timers.frame.get_avg() << '\n';
	go->shutdown();

	return 0;
}

int test_main()
{
	foo();
	return 0;
}
 
