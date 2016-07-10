
#include "os/compat.h"

#include <iostream>
#include <valarray>
#include <sstream>
#include <ctime>

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

using namespace reaper;

using std::deque;

using os::time::timespan;
using os::time::RelTime;

using namespace object;

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

using reaper::phys::Engine;

namespace id = os::event::id;
namespace ev = os::event;

struct Tree : public object::Tree2 {
	Tree(const Vector& v) : Tree2(SillyData(None,Matrix4(v))) { }
}; 
struct Tower : public object::Tower {
	Tower(const Vector& v) : object::Tower(SillyData(None,Matrix4(v))) { }
};

typedef Ship PlayerShip;

class TestShip : public PlayerShip
{
	PlayerID my_id;
	EventProxy ep;
	bool missile_select;
public:

	TestShip(PlayerID id, CompanyID i, const Vector& v)
	 : PlayerShip(SillyData(i, Matrix4(v))),
	   my_id(id), ep(EventSystem::get_ref(my_id)),
	   missile_select(false)
	{
	}
	void think() {
		ctrl.missile_fire = ep.key('M');
		ctrl.afterburner = ep.key('Q');
		ctrl.fire = ep.key(' ');		
		ctrl.pitch  = ep.axis(1);
		ctrl.yaw    = ep.axis(0);
		ctrl.thrust = ep.axis(3);
	
		if(!missile_select) {
			if(ep.key('N')) {
				ctrl.select_next_missile();
				derr << "next missile\n";
			} else if(ep.key('B')) { 
				ctrl.select_prev_missile();
				derr << "prev missile\n";
			}
			missile_select = true;
		} else { 
			missile_select = ep.key('N') || ep.key('B');
		}
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
		ctrl.yaw = 0.2;
		ctrl.thrust = .5;
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
	   td(1), 		ship_block(80),
	   conv_length(0), 	trees(70),
	   towers(66), 		ai_ships(94),
	   playback("")
	{ }

};

Config cnf;



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
	reaper::phys::PhysRef pr;

	Game(os::gfx::Gfx& gx) : game(gx), wr(0), mr(0), rr(0), pr(0) {  }
};

//Game go;

struct RndPoint : public Point {
	RndPoint(int xr, int yr, int zr, int xa = 0, int ya = 0, int za = 0)
	 : Point(rand() % xr + xa, rand() % yr + ya, rand() % zr + za)
	{ }
};

const char* turret_info  =
	"object: turret\n"
	"position: [-2055.579, 513.1, -2416.7]\n"
	"object: turret\n"
	"position: [-2172.5, 423, -2662.2]\n"
	"object: turret\n"
	"position: [-2114.7, 410.8, -2927.9]\n"
	"object: turret\n"
	"position: [-1977.3, 420.7, -3161.9]\n"
	"object: turret\n"
	"position: [-1663.6, 424.5, -3293.6]\n"
	"object: turret\n"
	"position: [-758.7, 309.3, -2689.4]\n"
	"object: turret\n"
	"position: [-140, 195.3, -2265.5]\n"
	"object: turret\n"
	"position: [0.7, 183.5, -2183.6]\n"
	"object: turret\n"
	"position: [405.4, 161.7, -2183.4]\n"
	"object: turret\n"
	"position: [856.3, 193.6, -1971.7]\n"
	"object: turret\n"
	"position: [1076.6, 217.5, -1679.9\n";

const char* dyn_obj_info =
	"object: ai_ship\n"
	"object: ship2\n"
	"position: [1200, 700, 600]\n"
	"waypoint: [0, 500, 0]\n"
	"waypoint: [1000, 600, -100]\n"
	"waypoint: [-200, 700, 200]\n"
	"object: ai_ship\n"
	"object: ship3\n"
	"position: [1250, 700, 600]\n"
	"waypoint: [50, 500, 0]\n"
	"waypoint: [1050, 600, -100]\n"
	"waypoint: [-250, 700, 200]\n"
	"object: ai_ship\n"
	"object: ship4\n"
	"position: [1200, 750, 600]\n"
	"waypoint: [0, 550, 0]\n"
	"waypoint: [1000, 650, -100]\n"
	"waypoint: [-200, 750, 200]\n"
	"object: ai_ship\n"
	"object: shipX\n"
	"position: [1200, 700, 650]\n"
	"waypoint: [0, 500, 50]\n"
	"waypoint: [1000, 600, -150]\n"
	"waypoint: [-200, 700, 250]\n"
	"object: ground_vehicle\n"
	"position: [1000, 250, 100]\n"
	"object: ground_vehicle\n"
	"position: [1020, 250, 100]\n"
	"object: ground_vehicle\n"
	"position: [1040, 250, 100]\n"
	"object: ground_vehicle\n"
	"position: [1000, 250, 110]\n"
	"object: ground_vehicle\n"
	"position: [1020, 250, 110]\n"
	"object: ground_vehicle\n"
	"position: [1040, 250, 110]\n";


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

void create_objs(Game &go)
{
	float z_pos = -40;
	for (int i = 0; i < go.ply_ids.size(); ++i, z_pos -= 30 ) {
		PlayerShip* s = new TestShip(go.ply_ids[i], (i == go.local_id) ? Slick : None,
						Point(1000, 600, z_pos));
		go.a_ships.push_back(s);
		go.pr->insert(s);
	}
	go.wr->set_local_player(go.a_ships[go.local_id]);

	for (int i = 0; i < cnf.ship_block; i++) {
		Point p(1000 + (i % 4) * 50, 600 + (i/16)*50, ((i%16) / 4) * 50);
		GenShip* s = 0;
		switch ((i/4) % 4) {
		case 0: s = new DummyShip<Ship>(p); break;
		case 1: s = new DummyShip<Ship3>(p); break;
		case 2: s = new DummyShip<Ship4>(p); break;
		case 3: s = new DummyShip<ShipX>(p); break;
		}
		go.dyn_objs.push_back(s);
		go.pr->insert(s);
	}

	for (int i = 0; i < cnf.conv_length; ++i) {
		Point p(-800 + (i%3)*40, 0, -1000 + (i/3)*40);
		p.y = go.wr->get_altitude(Point2D(p.x, p.z)) + 60;
		GroundContainer* g = new GroundContainer(SillyData(None, Matrix4(p)));
		go.dyn_objs.push_back(g);
		go.pr->insert(g);
	}
	
	object::factory::Factory& factory = object::factory::inst();

	factory.register_object("turret", object::factory::pos_loader<Turret>());
	factory.register_object("ship2", object::factory::pos_loader<Ship>());
	factory.register_object("ship3", object::factory::pos_loader<Ship3>());
	factory.register_object("ship4", object::factory::pos_loader<Ship4>());
	factory.register_object("shipX", object::factory::pos_loader<ShipX>());
	factory.register_object("ground_vehicle", object::factory::pos_loader<GroundContainer>());
	factory.register_object("ai_ship", load_ai_ship);


	std::istringstream is(turret_info);
	while (Turret* t = factory.load(is, TMap<Turret>())) {
		go.pr->insert(t);
		go.turrets.push_back(t);
	}

	for (int i = 0; i < cnf.trees; i++) {
		RndPoint rnd(1000, 1, 1000);
		rnd.y = go.wr->get_altitude(Point2D(rnd.x, rnd.z));
		Tree* t = new Tree(rnd);
		go.pr->insert(t);
	}
	for (int i = 0; i < cnf.towers; i++) {
		RndPoint rnd(1000, 1, 1000);
		rnd.y = go.wr->get_altitude(Point2D(rnd.x, rnd.z));
		::Tower* t = new ::Tower(rnd);
		go.pr->insert(t);
	}

	std::istringstream is_dyn(dyn_obj_info);
	DynamicBase* o;
	while (cnf.ai_ships-- > 0 && ((o = factory.load(is_dyn, TMap<DynamicBase>())) != 0)) {
		go.dyn_objs.push_back(o);
		go.pr->insert(o);
	}


	go.sheep = new object::Sheep(SillyData(None,Matrix4(true)));
	go.pr->insert(go.sheep);
}


void game_init(os::gfx::Gfx& gx, Game& go)
{
	debug::debug_disable("sockstream");
//	debug::debug_disable("snd_dummy");

	go.wr = world::World::create(cnf.level);
	go.pr = reaper::phys::Engine::create();

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

//	go.rr->use_texturing(cnf.texture == 1);
//	go.rr->use_lighting(cnf.lighting == 1);
//	go.rr->draw_sky(cnf.sky == 1);
//	go.rr->draw_terrain(cnf.terrain == 1);
//	go.rr->use_fog(cnf.fog == 1);
//	go.rr->draw_shadows(cnf.shadow == 1);
//	go.rr->draw_effects(cnf.effects == 1);
//	go.rr->draw_particles(cnf.particles == 1);
//	go.rr->texture_scaling(8);
//	go.rr->draw_stats(2);

//	go.rr->draw_hud(gfx::External_HUD);

//	go.mr = gfx::object::Manager::create();


	if (cnf.run_multi) {
		std::cout << "waiting... press <enter> to start\n";
		cin.get();
		if (! go.game.start_req()) {
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
	case 1:	c_pos = ship_pos - ship_vel/10 - normalize3(ship_vel) * 10 + Point(0,5,0);
		break;
	case 2:  {
		static valarray<Vector> prev(c_up,60);
		static int cnt = 0;
		static float dt = 0;
		static int maxval = prev.size();
		Point old_pos = poslist.back();
		c_at = ship_pos + normalize3(ship_vel)*10;
		c_pos = ship_pos - normalize3(ship_pos - old_pos) * 10 *
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

	return gfx::Camera(c_pos, c_at, c_up, 60, 45);


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
	glPutStr(0, 0, timers.ai.report_last());
	glPutStr(0, 2, timers.phys.report_last());
	glPutStr(0, 4, timers.gfx.report_last());
	glPutStr(0, 6, timers.io.report_last());
	glPutStr(0, 8, timers.snd.report_last());
	glPutStr(0, 10, timers.wait.report_last());
	glPutStr(0, 12, timers.world.report_last());

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
			case 'P': cnf.particles ^= 1;
				  go.rr->draw_particles(cnf.particles == 1);

			case 'I': reaper::gfx::screenshot();
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

int test_main()
{

	os::gfx::Gfx gx;

	Game* go = new Game(gx);

//	snd::SoundManager sm;
	game_init(gx, *go);

	EventProxy ep = EventSystem::get_ref(os::event::System);


	create_objs(*go);

	float td = 1.0;
	int cam = 0;

	gfx::Camera c(Point(0,0,0),Point(0,0,1),Vector(0,1,0),90,90);
	go->rr->render(c);
	gx->update_screen();

//	go->pr->reinit();

	timespan prevtime = os::time::get_time();

	bool quit = false;

	double time = 0;
	int frames = 0;

	timers.frame.start();
	RelTime sim_time = 0;
	while (!quit && !exit_now()) {

		{ Profile p(timers.io);
		  quit = process_events(ep, cam, *go);
		  misc::for_each(seq(go->a_ships), urk());
		}

                double timediff =  static_cast<double>(os::time::get_time() - prevtime) * 1e-6;
                prevtime = os::time::get_time();
		float dt = timediff;

		{ Profile p(timers.ai);
		  misc::for_each(seq(go->turrets), urk());
		  misc::for_each(seq(go->dyn_objs), urk());
		}

		{ Profile p(timers.phys);
		  go->pr->update_world(time,timediff);
		}

		{ Profile(timers.world);
		  go->wr->update();
		}

		//draw_timers();
		
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
//		  sm.set_engine(cam == 3);
//		  sm.set_camera(cm, go->a_ships[0]->get_velocity());
//		  sm.run();
		}

		time += timediff;
		frames++;
	}
	derr << "Successful exit. Average FPS = " << frames/time << '\n';

	return 0;
}
 
