

#include "os/compat.h"

#include "os/debug.h"
#include "os/gl.h"
#include "os/gfx.h"

#include "game/scheduler.h"
#include "game/helpers.h"
#include "game/progressbar.h"

#include "main/types_io.h"

#include "misc/command.h"
#include "misc/font.h"

#include "object/event.h"
#include "object/accessor.h"

#include "world/world.h"

#include <iomanip>

namespace reaper {
namespace game {

namespace {
	reaper::debug::DebugOutput derr("game::startup");
}

using misc::time::Profile;


void wait_net_display(const net::Players& ply, bool is_host)
{
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.5,0,0);
	gfx::font::glPutStr(0.1, 0.6, "Waiting for players to join", gfx::font::Medium, 0.02, 0.04);
	std::ostringstream msg;
	msg << "Joined: " << ply.size();
	gfx::font::glPutStr(0.1, 0.4, msg.str().c_str(), gfx::font::Medium, 0.02, 0.04);
	if (is_host) {
		gfx::font::glPutStr(0.1, 0.2, "Press enter to start", gfx::font::Medium, 0.02, 0.04);
	}
}


bool Game::game_start()
{
	loading->tick("startup");

	ply_id = 1;
	{ Profile p("init");
	  game.init(conf.networked, conf.servername, conf.split_screen);

	  ply_id = game.alloc_id(conf.observer);
	}
	wr->set_local_player(ply_id);

	sr->update(0.01);
 	wr->update(true);

	pr->update_world(-0.01, 0.01);
	object::DynamicPtr ply = wr->lookup_dyn(ply_id);

	ply->events().death.add_listener(misc::mk_cmd(this, &Game::player_death, ply->stat().id()));


	if (conf.split_screen) {
		object::DynamicPtr ply2;
		ply2.dynamic_assign(wr->lookup_dyn(game.alloc_id(false)));
		ply2->events().death.add_listener(misc::mk_cmd(this, &Game::player_death, ply2->stat().id()));
		ply2_id = ply2->stat().id();
	}

	loading->tick("done");
	if (gx.is_accelerated()) {
		for(float f = 0; f < 1.0; f += 0.01) {
			loading->render("done");
			fade(f);
			gx.update_screen();
		}
	}

	bool server_wait_start = true;
	if (conf.networked) {
		os::event::Event junk;
		while (sys_ep.get_event(junk))
			;
		while (!game.get_multistatus() && server_wait_start) {
			wait_net_display(game.players(), conf.server);
			if (conf.server) {
				os::event::Event e;
				while (sys_ep.get_event(e)) {
					using namespace os::event;
					if (e.id == id::Enter) {
						server_wait_start = false;
						break;
					} else if (e.id == id::Escape) {
						return false;
					}
				}
			}
			gx.update_screen();
			os::time::msleep(100);
		}
	}

	if (!game.start_req())
		derr << "Start failed!\n";

	// remove all player objects without attached player:
	const net::Players& plys = game.players();
	for (int i = 1; i < 20; ++i) {
		if (plys.find(i) == plys.end()) {
			// this is kind of silly, because i == id,
			// but that may change..
			object::DynamicPtr d = wr->lookup_dyn(i);
			if (d.valid()) {
				int id = d->stat().id();
				d->events().remove.ping_all();
			}
		}
	}
	wr->update();

	rr->start();

	game.go_go_go();
	loop_ticker.reset();
	start_time = os::time::get_time();

	timers.frame.start();

	sim_dt = 0;

	delete loading;
	loading = 0;
	return true;
}


}
}

