
#include "os/compat.h"

#include <iostream>
#include <valarray>
#include <sstream>

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
#include "game/scenario_mgr.h"
#include "misc/test_main.h"

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
using object::GroundShip;

using os::event::EventProxy;
using os::event::EventSystem;
using os::time::Profiler;
using net::PlayerID;
using misc::seq;
using misc::for_each;

using phys::Engine;

namespace id = os::event::id;
namespace ev = os::event;

typedef Ship PlayerShip;

class TestShip : public PlayerShip
{
	PlayerID my_id;
	EventProxy ep;
public:

	TestShip(PlayerID id, CompanyID i, const Vector& v)
	 : PlayerShip(SillyData(i, Matrix4(v))),
	   my_id(id), ep(EventSystem::get_ref(my_id))
	{
	}
	void think() {
		ctrl.fire = ep.key(' ');
		ctrl.pitch = ep.axis(1);
		ctrl.yaw = -ep.axis(0);
		ctrl.thrust = ep.axis(3);
	}
	void collide(object::CollisionInfo&) { }
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
	const char* level;
	bool record;
	bool run_multi;
	bool no_tex_compr;
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
	 : level("test_level"), record(false),
	   run_multi(false), 	no_tex_compr(true),
	   texture(1), 		lighting(1),
	   sky(1), 		terrain(1),
	   fog(1), 		shadow(1),
	   particles(1), 	effects(1),
	   td(1), 		ship_block(16),
	   conv_length(10), 	trees(10),
	   towers(10), 		ai_ships(0),
	   playback("")
	{ }

};

Config cnf;


gfx::RendererRef rr2(0);

struct Game {
	deque<PlayerShip*> a_ships;
	deque<DynamicBase*> dyn_objs;
	deque<object::Turret*> turrets;
	object::Sheep* sheep;

	net::GameMgr game;
	std::deque<PlayerID> ply_ids;
	int local_id;
	
	world::WorldRef wr;
	gfx::object::ManagerRef mr;
	gfx::RendererRef rr;
	phys::PhysRef pr;

	Game(os::gfx::Gfx& gx) : game(gx), wr(0), mr(0), rr(0), pr(0) {  }
};

struct RndPoint : public Point {
	RndPoint(int xr, int yr, int zr, int xa = 0, int ya = 0, int za = 0)
	 : Point(rand() % xr + xa, rand() % yr + ya, rand() % zr + za)
	{ }
};

void create_objs(Game& go)
{

	float z_pos = -40;
	for (int i = 0; i < 1; ++i, z_pos -= 30 ) {
		PlayerShip* s = new TestShip(go.ply_ids[i], (i == go.local_id) ? Slick : None,
						Point(-2530.78,440.819,-2673.12));
		go.a_ships.push_back(s);
		go.wr->add_object(s);
	}
	go.wr->set_local_player(go.a_ships[go.local_id]);

	//go.wr->add_object(s1);
}

void game_init(os::gfx::Gfx& gx, Game& go)
{
	debug::debug_disable("sockstream");
//	debug::debug_disable("snd_dummy");

	go.wr = world::World::create(cnf.level);
	go.pr = phys::Engine::create();

	net::PlayerID local_id;
	if (cnf.run_multi)
		local_id = go.game.init_multiplayer();
	else
		local_id = go.game.init_singleplayer();

	if (cnf.no_tex_compr) {
		gfx::Settings sts = gfx::Settings::defaults_using_extensions;
		sts.use_arb_texture_compression = false;
		sts.use_ext_s3tc_texture_compression = false;
		go.rr = gfx::Renderer::create(gx, &sts);	
	} else {
		go.rr = gfx::Renderer::create(gx);	
	}
	rr2 = gfx::Renderer::get_ref();	

	go.rr->use_texturing(cnf.texture == 1);
	go.rr->use_lighting(cnf.lighting == 1);
	go.rr->draw_sky(cnf.sky == 1);
	go.rr->draw_terrain(cnf.terrain == 1);
	go.rr->use_fog(cnf.fog == 1);
	go.rr->draw_shadows(cnf.shadow == 1);
	go.rr->draw_effects(cnf.effects == 1);
	go.rr->draw_particles(cnf.particles == 1);
//	go.rr->texture_scaling(8);
	go.rr->draw_stats(2);

	go.rr->draw_hud(gfx::External_HUD);

	go.mr = gfx::object::Manager::create();

//	go.tr = gfx::texture::Manager::get_ref();

	if (cnf.run_multi) {
		std::cout << "waiting... press <enter> to start\n";
		cin.get();
		if (! go.game.start()) {
			derr << "multistart failed\n";
			exit(1);
		}
		go.game.get_multistatus();
	}

	if (cnf.run_multi && go.game.remote_players().size() > 1) {
		std::vector<PlayerID>::const_iterator c, e = go.game.remote_players().end();
		int i = 0;
		for (c = go.game.remote_players().begin(); c != e; ++c, ++i) {
			go.ply_ids.push_back(*c);
			if (*c == local_id) {
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
	case 1:	c_pos = ship_pos - ship_vel/10 - normalize3(ship_vel) * 5 + Point(0,5,0);
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
		c_pos = ship_pos;
		c_at  = c_pos + ship_vel;
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
	Profiler phys, gfx, ai, wait, io, snd, frame, world;
	Timers()
	 : phys("Phys "), gfx("Gfx  "), ai("AI   "),
	   wait("Wait "), io("IO   "), snd("Snd  "),
	   frame("Frame "), world("World")
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

	glColor4f(0,0,1,.5);
	meter(.5,0, .5, 1, 1, timers.ai.get_last()/frame_length);
	meter(.5,2, .5, 1, 1, timers.phys.get_last()/frame_length);
	meter(.5,4, .5, 1, 1, timers.gfx.get_last()/frame_length);
	meter(.5,6, .5, 1, 1, timers.io.get_last()/frame_length);
	meter(.5,8, .5, 1, 1, timers.snd.get_last()/frame_length);
	meter(.5,10, .5, 1, 1, timers.wait.get_last()/frame_length);
	meter(.5,12, .5, 1, 1, timers.world.get_last()/frame_length);

	glColor4f(0,1,0,.5);
	meter(.5,1, .5, 1, 1, timers.ai.get_avg()/frame_length);
	meter(.5,3, .5, 1, 1, timers.phys.get_avg()/frame_length);
	meter(.5,5, .5, 1, 1, timers.gfx.get_avg()/frame_length);
	meter(.5,7, .5, 1, 1, timers.io.get_avg()/frame_length);
	meter(.5,9, .5, 1, 1, timers.snd.get_avg()/frame_length);
	meter(.5,11, .5, 1, 1, timers.wait.get_avg()/frame_length);
	meter(.5,13, .5, 1, 1, timers.world.get_avg()/frame_length);

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
			case '6': cnf.texture ^= 1;
				  go.rr->use_texturing(cnf.texture == 1);
				  break;
			case '7': cnf.lighting ^= 1;
				  go.rr->use_lighting(cnf.lighting == 1);
				  break;
			case '8': cnf.sky ^= 1;
				  go.rr->draw_sky(cnf.sky == 1);
				  break;
			case '9': cnf.terrain ^= 1;
				  go.rr->draw_terrain(cnf.terrain == 1);
				  break;
			case '0': cnf.fog ^= 1;
				  go.rr->use_fog(cnf.fog == 1);
				  break;
			case 'Y': cnf.shadow ^= 1;
				  go.rr->draw_shadows(cnf.shadow == 1);
				  break;
			case 'U': cnf.effects ^= 1;
				  go.rr->draw_effects(cnf.effects == 1);
				  break;
			case 'I': cnf.particles ^= 1;
				  go.rr->draw_particles(cnf.particles == 1);
				  break;

			case 'S': cnf.td -= 0.01;
				  if (cnf.td < 0.05)
					  cnf.td = 0.05;
				  go.rr->terrain_detail(cnf.td);
				  break;
			case 'X': cnf.td += 0.01;
				  if (cnf.td > 1)
					  cnf.td = 1;
				  go.rr->terrain_detail(cnf.td);
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

	sound::Manager sm;

	game_init(gx, *go);

	EventProxy ep = EventSystem::get_ref(os::event::System);

	game::scenario::ScenarioRef sr = game::scenario::ScenarioMgr::create();
	create_objs(*go);

	float td = 1.0;
	int cam = 0;

	gfx::Camera c(Point(0,0,0),Point(0,0,1),Vector(0,1,0),90,90);
	go->rr->render(c);
	gx->update_screen();

	

	os::time::msleep(1000);

        os::time::reset_rel_time();
        RelTime prevtime = os::time::get_rel_time();

        bool quit = false;

        double time = 0;
        int frames = 0;

        timers.frame.start();
        while (!quit && !exit_now()) {

		//derr << go->a_ships[0]->get_pos() << endl;

                { Profile p(timers.io);
                  quit = process_events(ep, cam, *go);
                  misc::for_each(seq(go->a_ships), urk());
                }

                double timediff = (os::time::get_rel_time() - prevtime) * 1e-3;
                prevtime = os::time::get_rel_time();
                float dt = timediff;

                { Profile p(timers.ai);
			sr->update(timediff);
		}

                { Profile p(timers.phys);
                  go->pr->update_world(time,timediff);
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

                { Profile p(timers.gfx);
                  go->rr->render(mk_cam(cam, timediff, *go));
                }
                { Profile p(timers.snd);
                  sm.set_engine(cam == 3);
                  sm.run();
                }

                time += timediff;
                frames++;
        }

	derr << "Successful exit. Average FPS = " << frames/time << '\n';

	go->wr->destroy();
	derr << "die!\n";
	delete go;
	return 0;
}

int test_main()
{
	int r = 0;
	char c = 'n';
	do {
		r = foo();	
		cout << "More?\n";
//		cin >> c;
	} while (c == 'y');
	return r;
}
 
