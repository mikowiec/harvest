
#include "os/compat.h"

#include <iostream>
#include <valarray>
#include <sstream>
#include <map>

#include "gfx/gfx.h"
#include "os/gfx.h"
#include "os/event.h"
#include "os/time.h"
#include "os/mapping.h"
#include "world/world.h"
#include "gfx/object.h"
#include "gfx/camera.h"
#include "gfx/settings.h"
#include "gfx/object_manager.h"
#include "gfx/texture_manager.h"
#include "object/base.h"
#include "object/object.h"
#include "object/factory.h"
#include "os/gl.h"
#include "os/debug.h"
#include "phys/engine.h"
#include "main/types_io.h"
#include "net/game_mgr.h"
#include "snd/sound_system.h"
#include "misc/font.h"
#include "object/renderer.h"
#include "object/base_data.h"
#include "misc/test_main.h"
#include "res/config.h"
#include "game/scenario_mgr.h"

using namespace reaper;

using std::deque;

using os::time::timespan;
using os::time::RelTime;

using object::DynamicBase;
using object::GenShip;
using object::Ship;
using object::Ship3;
using object::Ship4;
using object::ShipX;
using object::SillyData;
using object::Turret;
using object::GroundOrdinary;

using os::event::EventProxy;
using os::event::EventSystem;
using os::time::Profiler;
using net::PlayerID;
using misc::seq;
using misc::for_each;

using phys::Engine;

namespace id = os::event::id;
namespace ev = os::event;


void report_refs();

struct Tree : public object::Tree2 {
	Tree(const Vector& v) : Tree2(SillyData(None,Matrix4(v))) { }
}; 
struct Tower : public object::Tower {
	Tower(const Vector& v) : object::Tower(SillyData(None,Matrix4(v))) { }
};

typedef Ship PlayerShip;

class TestShip : public PlayerShip
{
	EventProxy ep;
public:

	TestShip(PlayerID id, CompanyID i, const Vector& v)
	 : PlayerShip(SillyData(i, Matrix4(v))),
	   ep(EventSystem::get_ref(id))
	{ }
	~TestShip() { derr << "ship dying!!!\n"; }
	void think() {
		namespace id = os::event::id;
		os::event::Event e;
		while (ep.get_event(e)) {
			bool dn = e.is_pressed();
			switch (e.id) {
			case ' ': ctrl.fire = dn;         break;
			case 'M': ctrl.missile_fire = dn; break;
			case 'Q': ctrl.afterburner = dn;  break;
			case 'N': if (dn) ctrl.select_next_missile(); break;
			case 'B': if (dn) ctrl.select_prev_missile(); break;
			case id::Axis0: ctrl.yaw = e.val;    break;
			case id::Axis1: ctrl.pitch = e.val;  break;
			case id::Axis3: ctrl.thrust = e.val; break;
			default: break;
			}
		}
	}
};

template<class P>
class DummyShip : public P
{
public:

	DummyShip(const Vector& v)
	 : P(SillyData(None, Matrix4(v)))
	{
		ctrl.yaw = 0.1;
	}
	void think() { }
	void collide(object::CollisionInfo&) { }
};


using reaper::os::event::Event;

struct Config {
	std::string level;
	bool record;
	bool run_multi;
	bool observer;
	int texture;
	int lighting;
	int sky;
	int terrain;
	int fog;
	int shadow;
	int particles;
	int effects;
	float td;
	int ship_block, conv_length, trees, towers, ai_ships;
	std::string playback;

	Config()
	 : lighting(1),
	   sky(1), 		terrain(1),
	   fog(1), 		shadow(1),
	   particles(1), 	effects(1),
	   td(1)
	{
		using res::withdefault;
		res::ConfigEnv cnf("hw_test_input");
		level	    = withdefault(cnf, "level", std::string("test_level"));
		record	    = withdefault(cnf, "record", false);
		run_multi   = withdefault(cnf, "run_multi", false);
		observer    = withdefault(cnf, "observer", false);
		texture	    = withdefault(cnf, "texture", true);
		playback    = withdefault(cnf, "playback", std::string(""));
		ship_block  = withdefault(cnf, "ship_block", 16);
		conv_length = withdefault(cnf, "conv_length", 9);
		trees	    = withdefault(cnf, "trees", 20);
		towers	    = withdefault(cnf, "towers", 20);
		ai_ships    = withdefault(cnf, "ai_ships", 4);
	}

};


struct Game {
	deque<PlayerShip*> a_ships;
	deque<DynamicBase*> dyn_objs;
	deque<object::Turret*> turrets;
	object::Sheep* sheep;


	net::GameMgr game;
	std::deque<PlayerID> ply_ids;
	int local_id;
	int local_id_tmp;
	
	object::GroundTurret* gv;
	
	world::WorldRef wr;
	gfx::object::ManagerRef mr;
	gfx::texture::ManagerRef tr;
	gfx::RendererRef rr;
	phys::PhysRef pr;
	game::scenario::ScenarioRef sr;
	Config cnf;

	Game(os::gfx::Gfx& gx) : game(gx), wr(0), mr(0), tr(0), rr(0), pr(0), sr(0) {  }
};

struct RndPoint : public Point {
	RndPoint(int xr, int yr, int zr, int xa = 0, int ya = 0, int za = 0)
	 : Point(rand() % xr + xa, rand() % yr + ya, rand() % zr + za)
	{ }
};

struct foo {

	DynamicBase* operator()(std::istream& is)
	{
		GenShip* s = object::factory::inst().load(is, TMap<GenShip>());
		while (is.peek() == 'w') {
			std::string var, val;
			misc::config_line(is, var, val);
			s->add_waypoint(read<Point>(val));
		}
		return s;
	}
} load_ai_ship;

void create_objs(Game& go)
{

	float z_pos = -40;
	for (int i = 0; i < 1; ++i, z_pos -= 30 ) {
		derr << "playership: " << go.ply_ids[i] << ' ' << (i == go.local_id) << '\n';
		PlayerShip* s = new TestShip(go.ply_ids[i], (i == go.local_id) ? Slick : None,
						Point(-2000,800, -3000));
		derr << "creating ship: " << s << '\n';
		go.a_ships.push_back(s);
		go.pr->insert(s);
		go.game.add_sync(s);
	}
	go.wr->set_local_player(go.a_ships[go.local_id]);

	object::GroundTurret* s1 = new object::GroundTurret(SillyData(Punkrocker, Matrix4(Point(-2100,480,-3000))));
	s1->add_waypoint(Point(-2643.3,5,-2863.58));
	s1->add_waypoint(Point(-2446.15,5,-3011.76));
	go.pr->insert(s1);
	go.gv = s1;

	go.gv->add_waypoint(Point(-2100,480,-3000));
}


void game_init(os::gfx::Gfx& gx, Game& go)
{
	debug::debug_disable("sockstream");
//	debug::debug_disable("snd_dummy");

	go.wr = world::World::create(go.cnf.level);
	go.pr = phys::Engine::create();

	go.local_id_tmp = net::init_game(go.game, go.cnf.run_multi, go.cnf.observer);

	go.rr = gfx::Renderer::create(gx);	

	go.rr->use_texturing(go.cnf.texture == 1);
	go.rr->use_lighting(go.cnf.lighting == 1);
	go.rr->draw_sky(go.cnf.sky == 1);
	go.rr->draw_terrain(go.cnf.terrain == 1);
	go.rr->use_fog(go.cnf.fog == 1);
	go.rr->draw_shadows(go.cnf.shadow == 1);
	go.rr->draw_effects(go.cnf.effects == 1);
	go.rr->draw_particles(go.cnf.particles == 1);
//	go.rr->texture_scaling(8);
//	go.rr->draw_stats(2);

	go.rr->draw_hud(gfx::External_HUD);

	go.mr = gfx::object::Manager::create();

	go.tr = gfx::texture::Manager::get_ref();

	go.tr->use("ship");
	go.tr->use("ship3");
	go.tr->use("ship4");
	go.tr->use("ship_x");
	go.tr->use("tree_stem");
	go.tr->use("tree_leaf");
	go.tr->use("tower");
};

void game_start(Game& go)
{
	net::start_game(go.game, go.cnf.run_multi, go.cnf.observer);

	if ((go.cnf.observer || go.cnf.run_multi) && go.game.remote_players().size() > 1) {
		std::vector<PlayerID>::const_iterator c, e = go.game.remote_players().end();
		int i = 0;
		for (c = go.game.remote_players().begin(); c != e; ++c, ++i) {
			go.ply_ids.push_back(*c);
			if (*c == go.local_id_tmp) {
				go.local_id = i;
			}
		}
	} else {
		go.ply_ids.push_back(1);
		go.ply_ids.push_back(2);
		go.local_id = 0;
	}
}


gfx::Camera mk_cam(int cam, double tdiff, Game& go)
{
	PlayerShip* ship = go.a_ships[go.local_id];
	Point ship_pos = ship->get_pos();
	Vector ship_vel = ship->get_velocity();

	Point c_pos;
	Point c_at = ship_pos;
	Vector c_up(0,1,0);

	static std::list<Point> poslist(20,ship_pos - normalize3(ship_vel) * 15);
	switch (cam) {
	case 1:	c_pos = go.gv->get_pos(); - go.gv->get_velocity()/10 - normalize3(go.gv->get_velocity()) * 5 + Point(0,5,0);
		break;
	case 2:  {
		static valarray<Vector> prev(c_up,60);
		static int cnt = 0;
		static float dt = 0;
		static int maxval = prev.size();
		Point old_pos = poslist.back();
		c_at = ship_pos + normalize3(ship_vel)*20;
		c_pos = ship_pos - normalize3(ship_pos - old_pos) * 5 *
				      (1+length3(ship_vel)/100) + Vector(0, 3, 0);

		prev[cnt++] = ship->get_data().m.get_row(1);
		if(dt > .5 || cnt > prev.size()) {
			maxval = cnt;
			cnt = 0;
			dt = 0;
		}

		dt += tdiff;

		Vector sum(0,0,0);
		for(int i=0;i<maxval;i++)
			sum += prev[i];

		c_up = ((sum / maxval) + Vector(0,1,0))/2;

	}
	break;
	case 3: {
		c_pos = ship_pos - ship->get_data().m.get_row(2);
		c_at  = c_pos - ship->get_data().m.get_row(2);
		c_up = normalize3(ship->get_data().m.get_row(1));
	}
	break;
	case 4: c_pos = go.turrets[0]->get_pos() + Vector(0, 20, 20); break;
	case 5: c_pos = go.turrets[2]->get_pos() + Vector(0, 20, 20); break;
	case 6:	c_at = go.sheep->get_pos();
		c_pos = c_at + Vector(20,20,20);
		break;
	case 7: c_at = go.turrets[2]->get_pos() + Vector(0, 100, 20);
		break;
	default: c_pos = ship_pos - Vector(0, -10, 10);
	}

	poslist.pop_back();
	poslist.push_front(ship_pos);

	if(ship->get_data().is_dead()) {
		static float angle = 0;
		angle += tdiff/3;
		c_at = ship_pos;
		c_pos = c_at + Vector(sin(angle)*50,20,cos(angle)*50);
	}

	return gfx::Camera(c_pos, c_at, c_up, 100, 75);


}

struct Timers {
	Profiler phys, gfx, ai, wait, io, snd, frame, world, net;
	Timers()
	 : phys("Phys "), gfx("Gfx  "), ai("AI   "),
	   wait("Wait "), io("IO   "), snd("Snd  "),
	   frame("Frame "), world("World"), net("Net  ")
	{ }
} timers ;

void draw_timers()
{
	using gfx::font::glPutStr;
	using gfx::misc::meter;
	float frame_length = 1e6/50.0;

	glPushAttrib(GL_ENABLE_BIT);      
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glColor4f(1,1,1,.5);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,1,0,80);

	glMatrixMode(GL_MODELVIEW);
	glPutStr(0, 0, timers.ai.report_last(), gfx::font::Medium, 0.008, 1.9);
	glPutStr(0, 2, timers.phys.report_last(), gfx::font::Medium, 0.008, 1.9);
	glPutStr(0, 4, timers.gfx.report_last(), gfx::font::Medium, 0.008, 1.9);
	glPutStr(0, 6, timers.io.report_last(), gfx::font::Medium, 0.008, 1.9);
	glPutStr(0, 8, timers.snd.report_last(), gfx::font::Medium, 0.008, 1.9);
	glPutStr(0, 10, timers.wait.report_last(), gfx::font::Medium, 0.008, 1.9);
	glPutStr(0, 12, timers.world.report_last(), gfx::font::Medium, 0.008, 1.9);
	glPutStr(0, 14, timers.net.report_last(), gfx::font::Medium, 0.008, 1.9);

	glColor4f(0,0,1,.5);
	meter(.5,0, .5, 1, 1, timers.ai.get_last()/frame_length);
	meter(.5,2, .5, 1, 1, timers.phys.get_last()/frame_length);
	meter(.5,4, .5, 1, 1, timers.gfx.get_last()/frame_length);
	meter(.5,6, .5, 1, 1, timers.io.get_last()/frame_length);
	meter(.5,8, .5, 1, 1, timers.snd.get_last()/frame_length);
	meter(.5,10, .5, 1, 1, timers.wait.get_last()/frame_length);
	meter(.5,12, .5, 1, 1, timers.world.get_last()/frame_length);
	meter(.5,14, .5, 1, 1, timers.net.get_last()/frame_length);

	glColor4f(0,1,0,.5);
	meter(.5,1, .5, 1, 1, timers.ai.get_avg()/frame_length);
	meter(.5,3, .5, 1, 1, timers.phys.get_avg()/frame_length);
	meter(.5,5, .5, 1, 1, timers.gfx.get_avg()/frame_length);
	meter(.5,7, .5, 1, 1, timers.io.get_avg()/frame_length);
	meter(.5,9, .5, 1, 1, timers.snd.get_avg()/frame_length);
	meter(.5,11, .5, 1, 1, timers.wait.get_avg()/frame_length);
	meter(.5,13, .5, 1, 1, timers.world.get_avg()/frame_length);
	meter(.5,15, .5, 1, 1, timers.net.get_avg()/frame_length);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();
}

template<class A>
bool process_events(A& ep, int& cam, Game& go) {
	Event e;
	if (ep.get_event(e)) {
		if (e.id < id::Btn0 && e.val > 0.0) {
			switch (e.id) {
			case id::Escape: return true;
			case id::F1: cam = 1; go.rr->draw_hud(gfx::External_HUD); break;
			case id::F2: cam = 2; go.rr->draw_hud(gfx::External_HUD); break;
			case id::F3: cam = 3; go.rr->draw_hud(gfx::Internal_HUD); break;
			case id::F4: cam = 4; go.rr->draw_hud(gfx::None_HUD); break;
			case id::F5: cam = 5; go.rr->draw_hud(gfx::None_HUD); break;
			case id::F6: cam = 6; go.rr->draw_hud(gfx::None_HUD); break;
			case id::F7: cam = 7; go.rr->draw_hud(gfx::None_HUD); break;
			case id::F8: cam = 8; go.rr->draw_hud(gfx::None_HUD); break;
			case '6': go.cnf.texture ^= 1;
				  go.rr->use_texturing(go.cnf.texture == 1);
				  break;
			case '7': go.cnf.lighting ^= 1;
				  go.rr->use_lighting(go.cnf.lighting == 1);
				  break;
			case '8': go.cnf.sky ^= 1;
				  go.rr->draw_sky(go.cnf.sky == 1);
				  break;
			case '9': go.cnf.terrain ^= 1;
				  go.rr->draw_terrain(go.cnf.terrain == 1);
				  break;
			case '0': go.cnf.fog ^= 1;
				  go.rr->use_fog(go.cnf.fog == 1);
				  break;
			case 'Y': go.cnf.shadow ^= 1;
				  go.rr->draw_shadows(go.cnf.shadow == 1);
				  break;
			case 'U': go.cnf.effects ^= 1;
				  go.rr->draw_effects(go.cnf.effects == 1);
				  break;
			case 'I': reaper::gfx::screenshot();
				  break;

			case 'S': go.cnf.td -= 0.01;
				  if (go.cnf.td < 0.05)
					  go.cnf.td = 0.05;
				  go.rr->terrain_detail(go.cnf.td);
				  break;
			case 'X': go.cnf.td += 0.01;
				  if (go.cnf.td > 1)
					  go.cnf.td = 1;
				  go.rr->terrain_detail(go.cnf.td);
				  break;
			default: break;
			}
		}
	}
	return false;
}

struct Profile {
	Profiler& p;
	Profile(Profiler& pp) : p(pp) { p.start(); }
	~Profile() { p.stop(); }
};

struct urk {
	template<class T>
	void operator()(T* t) {
		t->think();
	}
};

int foo()
{
	os::gfx::Gfx gx;

	Game* go = new Game(gx);

	if (args.size() > 0 && args[0] == "obs")
		go->cnf.observer = true;

	sound::Manager sm;

	game_init(gx, *go);
	game_start(*go);

	EventProxy ep = EventSystem::get_ref(os::event::System);


	create_objs(*go);
	//go->sr = game::scenario::ScenarioMgr::create(go->a_ships[0]);

	float td = 1.0;
	int cam = 0;

	gfx::Camera c(Point(0,0,0),Point(0,0,1),Vector(0,1,0),90,90);
	go->rr->render(c);
	gx->update_screen();

	os::time::msleep(1000);

	RelTime prevtime = os::time::get_rel_time();

	bool quit = false;

	double time = 0;
	int frames = 0;

	timespan real_time = os::time::get_time();

	timers.frame.start();
	RelTime sim_time = 0;
	while (!quit && !exit_now()) {
		long diff = (os::time::get_time() - real_time).approx() / 1000;
		sim_time += diff;
//		derr << "sim_time: " << sim_time << '\n';
		real_time = os::time::get_time();

		{ Profile p(timers.io);
		  quit = process_events(ep, cam, *go);
		  misc::for_each(seq(go->a_ships), urk());
		}

                double timediff = (os::time::get_rel_time() - prevtime) * 1e-3;
                prevtime = os::time::get_rel_time();
		float dt = timediff;

		const int step = diff;
		while (sim_time > step) {
			double ti = prevtime / 1000.0;
//			derr << "simulating at: " << ti << " for " << sim_time << '\n';

			{ Profile p(timers.ai);
			  //go->sr->update(timediff);
			  go->gv->think();
			}
			{ Profile p(timers.phys);
			  go->pr->update_world(ti, step * 1e-3);
			}
			sim_time -= step;
			os::time::rel_time_tick(step);
		}
//		  derr << "running phys at: " << time << " for " << timediff << "  -  " << prevtime / 1000.0 << " " << sim_time << '\n';
//		  phys_en.update_world(time, timediff);

		{ Profile(timers.net);
		  go->game.update();
//		  if ((frames % 10) == 0)
//			  derr << "current time: " << os::time::get_time() << "  rel: " << os::time::get_rel_time() << '\n';
		}

		{ Profile(timers.world);
		  go->wr->update();
		}

		draw_timers();
		
		{ Profile p(timers.wait);
		  gx->update_screen();
		}

		timers.frame.stop();
		timers.frame.start();
		
		gfx::Camera cm = mk_cam(cam, timediff, *go);
		{ Profile p(timers.gfx);
                  go->rr->render(cm);
		}
		{ Profile p(timers.snd);
		  sm.set_engine(cam == 3);
		  sm.set_camera(cm, go->a_ships[0]->get_velocity());
		  sm.run();
		}

		time += timediff;
		frames++;
	}
	derr << "Successful exit. Average FPS = " << frames/time << '\n';
	report_refs();
	go->sr->destroy();
	go->wr->destroy();
	report_refs();
	derr << "die!\n";
	delete go;
	report_refs();
	return 0;
}

int test_main()
{
	int r = 0;
	char c = 'n';
	do {
		r = foo();	
//		report_refs();
		cout << "More?\n";
		cin >> c;
	} while (c == 'y');
	return r;
}
 
