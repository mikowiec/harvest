/*
 *
 * Mainloop using the new scheduler, needs to be
 * further cleaned up, and eventually getting rid
 * of the gigantic Game-structure.
 *
 */


#include "os/compat.h"

#include "os/debug.h"
#include "os/gl.h"
#include "os/gfx.h"

#include "game/scheduler.h"
#include "game/helpers.h"

#include "gfx/camera.h"

#include "main/types_io.h"

#include "misc/command.h"
#include "misc/profile.h"
#include "misc/font.h"

#include "object/event.h"
#include "object/accessor.h"

#include "world/world.h"

#include <iomanip>

namespace reaper {
namespace game {

namespace {
	reaper::debug::DebugOutput derr("game::mainloop");
}


using misc::time::Profile;

class FrameControlJob : public Job
{
	int& timescale;
	bool& pause;
	misc::time::Ticker ticker;
public:
	FrameControlJob(int& ts, bool& p)
	 : timescale(ts), pause(p) { }
	bool work(Scheduler& s, RelTime dt) {
		ticker.tick(timescale / 1000.0);
		RelTime frame_dt = ticker.last_ms();
		s.tick_frame();
		if (!pause)
			s.tick_time(frame_dt);
		return true;
	}
};

class MaxFrameControlJob : public Job
{
	FrameControlJob real;
	int max_frames;
public:
	MaxFrameControlJob(int& ts, bool& pause, int m)
	 : real(ts, pause), max_frames(m) { }
	bool work(Scheduler& s, RelTime dt) {
		if (s.get_frames() >= max_frames) {
			s.clear();
			return false;
		}
		return real.work(s, dt);
	}
};

class ProcessEvents : public Job
{
	Game* game;
public:
	ProcessEvents(Game* g) : game(g) { }
	bool work(Scheduler& s, RelTime) {
		if (game->process_events(s)) {
			s.clear();
			return false;
		}
		return true;
	}
};

class RenderBase : public Job
{
	Game* game;
public:
	RenderBase(Game* g) : game(g) { }
	virtual Point render_frame(float dt) = 0;

	bool work(Scheduler& s, RelTime dt) {
		glEnable(GL_DEPTH_TEST);
		{ Profile p(game->timers.wait);
			game->gx.update_screen();
		}

		game->timers.frame.stop();
		game->timers.frame.start();

		game->cam_pos = render_frame(dt / 1000.0);
		glDisable(GL_DEPTH_TEST);
		return true;
	}
};

enum Frame { Left, Right, Full };

class RenderFrame : public RenderBase
{
	Game* game;
	Frame frame;
	object::ID id;
	object::DynamicPtr ply;
	int x, y, w, h;
	bool is_dead;
	Point last_pos;

	void setup_frame()
	{
		os::gfx::VideoMode vm = game->gx.current_mode();
		h = vm.height;
		y = 0;
		switch (frame) {
		case Full:  x = 0; w = vm.width; break;
		case Left:  x = 0; w = vm.width/2; break;
		case Right: x = vm.width/2; w = vm.width/2; break;
		}
	}
	void to_dead_cam()
	{
		last_pos = ply->stat().pos();
		ply = object::DynamicPtr(0);
		is_dead = true;
	}
public:
	RenderFrame(Game* g, Frame f, object::ID i)
	 : RenderBase(g), game(g), frame(f), id(i),
	   ply(game->wr->lookup_dyn(id)),
	   is_dead(false),
	   last_pos(ply->stat().pos())
	{
		setup_frame();
		ply->events().death.add_listener(
			misc::mk_cmd(this, &RenderFrame::to_dead_cam));
	}
	Point render_frame(float dt)
	{
		Profile p(game->timers.gfx);
		gfx::Camera cam;
		if (is_dead)
			cam = mk_dead_cam(last_pos, game->cam, dt);
		else
			cam = mk_cam(ply->stat().mtx(), ply->dyn().velocity(), game->cam, dt);
		if (frame != Full)
			cam.horiz_fov /= 2;
		setup_frame();
		glViewport(x, y, w, h);
		glScissor(x, y, w, h);
		glEnable(GL_SCISSOR_TEST);
		game->rr->render(cam);
		glDisable(GL_SCISSOR_TEST);
		return cam.pos;
	}
};

struct PrintMtx
{
	void operator()() {
		derr << world::WorldRef()->get_local_player()->stat().mtx() << '\n';
	}
};

struct PrintScale
{
	int& timescale;
	PrintScale(int& ts) : timescale(ts) { }
	void operator()() {

		if (timescale != 1000.0) {
			stringstream ss;
			ss << "Time scaled by: " << std::fixed << std::setw(5)
				<< std::setprecision(3) << timescale / 1000.0;

			setup_ortho();
			gfx::font::glPutStr(0.5, 0.9, ss.str().c_str());
			restore_ortho();

		}
		
	}
};

void simulate(Game* game, RelTime time_ms, RelTime dt_ms)
{
	double now_s = time_ms / 1000.0;
	double dt_s  = dt_ms / 1000.0;
	{ Profile p(game->timers.ai);
		game->sr->update(dt_s);
		game->think_ref->think();
	}
	{ Profile p(game->timers.phys);
	  game->pr->update_world(now_s, dt_s);
	  os::time::rel_time_tick(dt_ms);
	}
	game->rr->simulate(dt_s);
}


class SimulateFixed : public Job
{
	Game* game;
public:
	SimulateFixed(Game* g) : game(g) { }
	bool work(Scheduler& s, RelTime dt) {
		simulate(game, s.get_time(), dt);
		return true;
	}
};

class Simulate : public Job
{
	Game* game;
public:
	Simulate(Game* g) : game(g) { }
	bool work(Scheduler& s, RelTime dt) {
		const RelTime max_dt = 50;

		RelTime time_ms = s.get_time();
		RelTime delta_time_ms = dt;

		while (delta_time_ms > max_dt) {
			simulate(game, time_ms, max_dt);
			delta_time_ms -= max_dt;
			time_ms += max_dt;
		}

		simulate(game, time_ms, delta_time_ms);

		return true;
	}
};


class Sound : public Job
{
	Game* game;
	object::DynamicPtr ply;
public:
	Sound(Game*g)
	 : game(g), ply(game->wr->lookup_dyn(game->ply_id))
	{ }
	bool work(Scheduler&s, RelTime dt) {
		Profile p(game->timers.snd);
		game->sm->set_camera(game->cam_pos, ply->dyn().velocity(),
						    ply->dyn().velocity());
		game->sm->run();
		return true;
	}
};


using misc::mk_cmd;

void main_loop(Config& conf, Game* game)
{
	Scheduler sched;
	if (conf.max_frames > 0)
		sched.add_frame(
			new MaxFrameControlJob(game->timescale, game->pause,
					       conf.max_frames), 1);
	else
		sched.add_frame(
			new FrameControlJob(game->timescale, game->pause), 1);

	sched.add_frame(new ProcessEvents(game), 1);

	if (conf.render) {
		if (conf.split_screen) {
			sched.add_frame(new RenderFrame(game, Left,  game->ply_id), 1);
			sched.add_frame(new RenderFrame(game, Right, game->ply2_id), 1);
		} else {
			sched.add_frame(new RenderFrame(game, Full, game->ply_id), 1);
		}
	}
	sched.add_frame(new MiscSwitchCmd(mk_cmd(&game->fps, &gfx::FPSMeterVP::render), game->draw_fps), 1);
	sched.add_frame(new MiscSwitchCmd(mk_cmd(game, &Game::render_stats), conf.draw_timers), 1);

	if (conf.print_timers)
		sched.add_frame(new MiscCmd(mk_cmd(game, &Game::print_stats)), 1);
	if (conf.print_mtx)
		sched.add_frame(new MiscCmd(mk_cmd(PrintMtx())), 50);
	sched.add_frame(new MiscCmd(mk_cmd(PrintScale(game->timescale))), 1);
	sched.add_frame(new MiscCmd(mk_cmd(&*game->sr, &scenario::ScenarioMgr::draw_info)), 1);

	if (conf.fixed_dt) {
		sched.add_time(new SimulateFixed(game), conf.step_dt);
		sched.add_time(new MiscCmd(mk_cmd(&game->game, &net::GameMgr::update)), conf.step_dt);
	} else {
		sched.add_frame(new Simulate(game), 1);
		sched.add_frame(new MiscCmd(mk_cmd(&game->game, &net::GameMgr::update)), 1);
	}
	sched.add_frame(new MiscCmd(mk_cmd(&game->wr.inst(), &world::World::update, false)), 1);
        sched.add_frame(new Sound(game), 5);

	sched.run();
}

}
}

