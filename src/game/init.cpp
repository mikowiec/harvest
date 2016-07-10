
#include "os/compat.h"

#include "os/debug.h"
#include "os/gl.h"
#include "os/gfx.h"

#include "game/scheduler.h"
#include "game/helpers.h"
#include "game/progressbar.h"

#include "gfx/abstracts.h"
#include "gfx/settings.h"
#include "gfx/managers.h"

#include "main/types_io.h"

#include "misc/command.h"
#include "misc/font.h"
#include "misc/parse.h"
#include "misc/profile.h"

#include "object/objectgroup.h"
#include "object/factory.h"

#include "res/config.h"
#include "res/resource.h"

#include "world/world.h"
#include "game/level.h"

#include <iomanip>

namespace reaper {
namespace game {

namespace {
	reaper::debug::DebugOutput derr("game::init");
	reaper::debug::DebugOutput dlog("game::init", 5);
}

using namespace res;
using namespace misc;

Config::Config(const Args& args)
 : draw_timers(false)
{
	Args::const_iterator c, e = args.end();
	for (c = args.begin(); c != e; ++c) {
		stringpair a = split(c->second, ':');
		if (! (a.first.empty() || a.second.empty())) {
			stringpair b = split(a.second, '=');
			resource<ConfigEnv>(a.first)[b.first] = b.second;
		}
	}		

	reload();

}

void Config::reload()
{
	ConfigEnv cnf("game");

	level     = withdefault(cnf, "level", "level1");
	record    = withdefault(cnf, "record", false);
	networked = withdefault(cnf, "networked", false);
	server    = withdefault(cnf, "server", false);
	observer  = withdefault(cnf, "observer", false);
	playback  = withdefault(cnf, "playback", "");
	scenario  = withdefault(cnf, "scenario", "");
	fixed_dt  = withdefault(cnf, "fixed_dt", false);
	hud       = withdefault(cnf, "hud", true);
	step_dt   = withdefault(cnf, "step_dt", 64);
	render    = withdefault(cnf, "render", true);
	print_timers = withdefault(cnf, "print_timers", !render);
	print_mtx = withdefault(cnf, "print_mtx", false);
	max_frames = withdefault(cnf, "max_frames", 0);

	split(withdefault(cnf, "player_ships", "shipXP"),
			std::back_inserter(player_ships));

	split_screen = withdefault(cnf, "split_screen", false);
	player_name  = withdefault(cnf, "player_name", "Player 1");
	servername   = withdefault(cnf, "servername", "localhost");
}


void load_mesh(const string& mesh)
{
	dlog << "preload mesh: " << mesh << '\n';
	gfx::MeshRef()->load(mesh);
}

void load_tex(const string& tex)
{
	dlog << "preload texture: " << tex << '\n';
	gfx::TextureRef()->load(tex);
}


void Game::game_init()
{
	loading->render("init");
	if (gx.is_accelerated()) {
		for (float f = 1.0; f >= 0; f -= 0.04) {
			loading->render("init");
			fade(f);
			gx.update_screen();
		}
	}

	if (! gx.is_accelerated()) {
		rr->settings().texture_level = 0;
		rr->settings().use_fog = false;
		rr->settings().draw_sky = false;
	}
	loading->tick("init");
	if (conf.server)
		game.start_server();
	loading->tick("world");
	{ time::Profile p("world");
	  wr->load(conf.level);
	}
	loading->tick("phys");
	{ time::Profile p("phys");
	  pr.inst();
	}
	think_ref.inst();

	if (! conf.playback.empty())
		game.playback_game(conf.playback);
	else if (conf.record)
		game.record_game(true);

	loading->tick("level");
	{ time::Profile p("rend");
	  rr->load();
	  rr->settings().hud_type = gfx::External_HUD;
	  rr->settings().radar_range = 1600;
	}

	loading->tick("settings");
	{ time::Profile p("rest");
 	  wr->update(true);
	  apply_settings();
	}

	loading->tick("sounds");
	{ time::Profile p("snd");
	  sm = reaper::sound::Manager::create();
	}

	loading->tick("scenario");
	{ time::Profile scp("scen_mgr");
	  std::string sc = conf.scenario;
	  if (sc.empty())
	  	sc = wr->get_level_info().scenario;
	  if (sc.empty())
	  	sc = "none";
	  sr = game::scenario::ScenarioRef::create();

	  sr->init(sc);
	}

	loading->tick("objects");
	{ time::Profile p("objects");
	  gfx::MeshRef mr;
	  gfx::TextureRef tr;

	  set<string> obj_names;
	  sr->get_objectnames(obj_names);

	  set<string>::iterator c, e = obj_names.end();
	  set<string> meshes, textures;
	  for (c = obj_names.begin(); c != e; ++c) {
		  try {
			string obj = *c;
			misc::Unique tex = mr->get_texture(gfx::RenderInfo(obj, Matrix::id(), false));
			meshes.insert(obj);
			if (!tex.get_text().empty())
				textures.insert(tex.get_text());
		  } catch (res::resource_not_found) {
		  	/* some custom meshes will not be found here,
			 * so ignore errors for now..
			 */
		  }
	  }
	  for_each(seq(meshes), load_mesh);
	  for_each(seq(textures), load_tex);
	}
}



}
}

