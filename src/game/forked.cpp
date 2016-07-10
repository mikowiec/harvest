
#include "os/compat.h"

#include "os/debug.h"

#include "os/gfx.h"
#include "os/concurrent.h"
#include "game/menus.h"
#include "game/helpers.h"
#include "game/mainloop.h"
#include "res/res.h"

#include "game/forked.h"

namespace reaper {

class Reaper::Run : public os::concurrent::Runnable
{
	os::gfx::Gfx* gx;
	game::Game* go;
	bool quit_req, done;
	os::concurrent::Semaphore init_done;
	std::string error;
public:
	Run() : gx(0), go(0), quit_req(false), done(false)
	{
	}
	void run()
	{
//		debug::debug_disable();
		debug::debug_priority(-1);
		
		try {
			auto_ptr<os::gfx::Gfx> gx(new os::gfx::Gfx());
			Args args;
			auto_ptr<game::Game> go(new game::Game(*gx, args));
			init_done.signal();
			go->game_init();
			go->game_start();
			main_loop(go->conf, go.get());

			done = true;
			return;
		}
		catch (error_base& e) {
			error = e.what();
		}
		catch (std::exception& e) {
			error = e.what();
		}
		catch (...) {
			error = "unknown error\n";
		}
		done = true;
		init_done.signal();
	}
	void start_wait()
	{
		init_done.wait();
	}
	void req_stop()
	{
		quit_req = true;
	}
	std::string get_errmsg()
	{
		return error;
	}
	bool is_done()
	{
		return done;
	}
};

Reaper::Reaper(const std::string& root)
 : run(new Run()),
   gth(new os::concurrent::Thread(run))
{
	if (!root.empty()) {
		res::add_datapath(root + "/data/");
		res::add_pluginpath(root + "/plugins/");
	}
}

bool Reaper::start()
{
	gth->start();
	run->start_wait();
	return run->get_errmsg().empty();
}

void Reaper::stop()
{
	run->req_stop();
	gth->stop(true);
	delete gth;
	delete run;
	run = 0;
}

bool Reaper::is_done()
{
	return run->is_done();
}

std::string Reaper::get_errmsg()
{
	return run->get_errmsg();
}

}


extern "C"
reaper::Reaper* create_reaper(void*) {
	return new reaper::Reaper();
}


