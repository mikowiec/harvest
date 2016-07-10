#ifndef REAPER_GAME_HELPERS_H
#define REAPER_GAME_HELPERS_H

#include <map>

#include "game/scenario_mgr.h"
#include "gfx/fps_meter.h"
#include "gfx/gfx.h"
#include "os/reltime.h"
#include "os/abstime.h"
#include "misc/profile.h"
#include "net/game_mgr.h"
#include "phys/engine.h"
#include "snd/sound_system.h"
#include "world/worldref.h"
#include "ai/thinker.h"

#include "gfx/cloud/cloud.h"

namespace reaper {
namespace {
        typedef std::map<std::string, std::string> Args;
}

namespace game {

class Scheduler;
struct Job;


void setup_ortho();
void restore_ortho();
void fade(float f);
gfx::Camera mk_cam(Matrix ship_mtx, Vector vel, int cam, double tdiff);
gfx::Camera mk_dead_cam(Point ship_pos, int cam, double tdiff);


struct Timers {
	misc::time::Profiler2 phys, gfx, ai, wait, io, snd, world, net;
	misc::time::Profiler frame;
	Timers();

	void draw_txt();
	void draw_last(float frame_length);
	void draw_avg(float frame_length);
	void draw_history(float frame_length);

	void print_txt();
};

struct Config {
	std::string level;
	bool record;
	bool networked;
	bool server;
	bool observer;
	std::string playback;
	std::string scenario;
	bool fixed_dt;

	bool draw_timers;
	bool hud;
	int step_dt;
	bool render;
	bool print_timers;
	bool split_screen;
	std::string servername;
	std::vector<std::string> player_ships;

	std::string player_name;

	bool clouds;
	int max_frames;

	bool print_mtx;

	Config(const Args& args);

	void reload();
};

class ProgressBar;

struct Game
{
	Config conf;
	os::gfx::Gfx& gx;
	
	gfx::RendererRef rr;

	world::WorldRef wr;
	reaper::sound::SoundRef sm;
	reaper::phys::PhysRef pr;
	game::scenario::ScenarioRef sr;
	object::ID ply_id, ply2_id;
	net::GameMgr game;

	ai::ThinkRef think_ref;

	Timers timers;
	misc::time::Ticker loop_ticker;
	os::event::EventProxy sys_ep;
	int cam;

	os::time::RelTime sim_dt;

	int timescale;
	bool pause;

	gfx::FPSMeterVP fps;
	bool draw_fps;

	ProgressBar* loading;

	os::time::TimeSpan start_time;

	Point cam_pos;

	std::set<object::ID> pl_dead;
	void player_death(object::ID id);

	void apply_settings();
	
	Game(os::gfx::Gfx& gx, const Args& args);
	~Game();

	void game_init();

	bool game_start();

	bool process_events(Scheduler&);

	void shutdown();

	void render_stats();
	void print_stats();
};


}
}

#endif

