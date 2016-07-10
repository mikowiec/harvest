
#include "os/compat.h"


#include "game/helpers.h"
#include "game/helpers.h"
#include "game/progressbar.h"
#include "game/scheduler.h"
#include "gfx/camera.h"
#include "gfx/misc.h"
#include "gfx/settings.h"
#include "main/types_io.h"
#include "main/types_ops.h"
#include "misc/font.h"
#include "object/objectgroup.h"
#include "os/debug.h"
#include "os/extra.h"
#include "os/gfx.h"
#include "os/gl.h"
#include "res/config.h"
#include "res/resource.h"

#include "world/world.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>


namespace reaper {
namespace object {
	int get_obj_count();
}
namespace game {

namespace {
	reaper::debug::DebugOutput derr("game::helpers");
	reaper::debug::DebugOutput dlog("game::helpers", 5);
}

void setup_ortho() {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
        glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
}

void restore_ortho() {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

using namespace res;
using namespace misc;
using std::string;
using os::time::RelTime;
using os::time::get_rel_time;


Timers::Timers()
 : phys("Phys ", 0, 15),
   gfx("Gfx  ", 0, 3),
   ai("AI   ", 0, 5),
   wait("Wait ", 0, 7),
   io("IO   ", 0, 9),
   snd("Snd  ", 0, 11),
   world("World", 0, 13),
   net("Net  ", 0, 1),
   frame("Frame ")
{ }

// Gah!

void Timers::draw_txt()
{
	phys.draw_txt();
	gfx.draw_txt();
	ai.draw_txt();
	wait.draw_txt();
	io.draw_txt();
	snd.draw_txt();
	world.draw_txt();
	net.draw_txt();
}

class Foo { } foo;

std::ostream& operator<<(std::ostream& os, const Foo&) {
	os << std::right << std::fixed << std::setprecision(0) << std::setw(5);
	return os;
}

void Timers::print_txt()
{
	derr << "phys: "  << foo << phys.get_last()  << " (" << foo << phys.get_avg() << ") ";
	derr << "gfx: "   << foo << gfx.get_last()   << " (" << foo << gfx.get_avg() << ") ";
	derr << "ai: "    << foo << ai.get_last()    << " (" << foo << ai.get_avg() << ") ";
	derr << "io: "    << foo << io.get_last()    << " (" << foo << io.get_avg() << ") ";
	derr << "snd: "   << foo << snd.get_last()   << " (" << foo << snd.get_avg() << ") ";
	derr << "world: " << foo << world.get_last() << " (" << foo << world.get_avg() << ") ";
	derr << "net: "   << foo << net.get_last()   << " (" << foo << net.get_avg() << ") ";
	derr << '\n';
}

void Timers::draw_last(float fl)
{
	phys.draw_last(fl);
	gfx.draw_last(fl);
	ai.draw_last(fl);
	wait.draw_last(fl);
	io.draw_last(fl);
	snd.draw_last(fl);
	world.draw_last(fl);
	net.draw_last(fl);
}

void Timers::draw_avg(float fl)
{
	phys.draw_avg(fl);
	gfx.draw_avg(fl);
	ai.draw_avg(fl);
	wait.draw_avg(fl);
	io.draw_avg(fl);
	snd.draw_avg(fl);
	world.draw_avg(fl);
	net.draw_avg(fl);
}

void Timers::draw_history(float fl)
{
	glPointSize(0.5);
	glColor3f(1,1,1);
	phys.draw_history(fl);
	glColor3f(1,0,0);
	gfx.draw_history(fl);
	glColor3f(1,1,1);
	ai.draw_history(fl);
	glColor3f(1,0,0);
	wait.draw_history(fl);
	glColor3f(1,1,1);
	io.draw_history(fl);
	glColor3f(1,0,0);
	snd.draw_history(fl);
	glColor3f(1,1,1);
	world.draw_history(fl);
	glColor3f(1,0,0);
	net.draw_history(fl);
}


void fade(float f)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	StateKeeper s1(GL_BLEND, true);
	glColor4f(0,0,0,f);
	glRectf(0,0,1,1);
}

Game::Game(os::gfx::Gfx& g, const Args& args)
 : conf(args),
   gx(g),
   game(gx),
   sys_ep(os::event::EventSystem::get_ref(os::event::System)),
   cam(2), sim_dt(0), timescale(1000), pause(false), draw_fps(false),
   loading(new ProgressBar(gx, "Loading: ", 10))
{
	rr = gfx::RendererRef::create();
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT);

	reaper::phys::Impl::prepare();
	reaper::ai::Impl::prepare();
	reaper::world::Impl::prepare();
}


Game::~Game()
{
	derr << "objects before ~Game: " << object::get_obj_count() << '\n';
	game::scenario::ScenarioRef::destroy();
	reaper::sound::SoundRef::destroy();
	reaper::phys::Impl::shutdown();
	rr->shutdown();
	world::Impl::shutdown();
	reaper::ai::Impl::shutdown();
	gfx::RendererRef::destroy();
	derr << "objects after ~Game: " << object::get_obj_count() << '\n';
}

void Game::render_stats()
{
	using gfx::font::glPutStr;
	float frame_length = 1e6/50.0;

	StateKeeper s2(GL_DEPTH_TEST, false);
	StateKeeper s3(GL_LIGHTING, false);

	glColor4f(1,1,1,.5);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,1,0,90);

	glMatrixMode(GL_MODELVIEW);

	char buf[100];
	snprintf(buf, 100,
		 "si: %3d st: %3d dyn: %3d shot: %3d  pairs: %4d  objs: %3d",
		 wr->silly_size(), wr->static_size(), 
		 wr->dynamic_size(), wr->shot_size(), pr->size(), object::get_obj_count());
	glPutStr(0, 17, buf, gfx::font::Medium, .008, 1.9);

	timers.draw_txt();

	StateKeeper s1(GL_TEXTURE_2D,false);
	glColor4f(0,0,1,.5);
	timers.draw_last(frame_length);

	glColor4f(0,1,0,.5);
	timers.draw_avg(frame_length);

	timers.draw_history(frame_length);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Game::print_stats()
{
	char buf[100];
	snprintf(buf, 100,
		 "si: %3d st: %3d dyn: %3d shot: %3d  pairs: %4d  objs: %3d",
		 wr->silly_size(), wr->static_size(), 
		 wr->dynamic_size(), wr->shot_size(), pr->size(), object::get_obj_count());
	derr << buf << ' ';
	timers.print_txt();
}


void Game::apply_settings()
{
	rr->settings().hud_type = conf.hud ? gfx::External_HUD : gfx::None_HUD;
}

using misc::time::Profile;


class PlayerObjectSet : public NodeConfig<object::ObjectGroup>
{
public:
	typedef tp<object::ObjectGroup>::ptr Ptr;
	Ptr create(IdentRef id) {
		return resource_ptr<object::ObjectGroup>(id);

	}

};

gfx::Camera mk_dead_cam(Point ship_pos, int cam, double tdiff)
{
	world::WorldRef wr;
	static float angle = 0;
	angle += tdiff/3;
	Point c_at = ship_pos;
	Point c_pos = c_at + Vector(sin(angle)*30, 20, cos(angle)*30);
	float alt = wr->get_altitude(Point2D(c_pos.x, c_pos.z));
	c_pos.y = max(c_pos.y, alt+2);

	Vector c_up = Vector(0,1,0);
	return gfx::Camera(c_pos, c_at, c_up, 100, 75);
}


gfx::Camera mk_cam(Matrix mtx, Vector vel, int cam, double tdiff)
{
	world::WorldRef wr;
	Point ship_pos = mtx.pos();

	float speed = length(vel);
	Vector org(0, 0, -1);
	Vector dir(mtx * org);
	if (speed > 1.0)
		dir *= speed;

	Point c_pos;
	Point c_at = ship_pos;
	Vector c_up(0,1,0);
	int up_sign = dot(Vector(0,1,0), mtx.col(1)) >= 0 ? 1 : -1;

	switch (cam) {
	case 1:	c_pos = ship_pos - dir/30 - norm(dir) * 15 + Point(0,2,0);
		c_up = Vector(0, up_sign, 0);
		break;
	case 2:
		c_at = ship_pos + norm(dir)*20;
		c_pos = ship_pos - dir/30 - norm(dir) * 15 + Point(0,2,0);
		c_up = Vector(0, up_sign, 0);
		break;
	case 3:
		c_pos = ship_pos - mtx.col(2);
		c_at  = c_pos - mtx.col(2);
		c_up = norm(mtx.col(1));
		break;
	case 4: c_pos = ship_pos - Vector(0, -10, 10);break;
	case 5: c_pos = ship_pos - 10*norm(dir);  break;
	case 6: c_pos = ship_pos - Vector(0, 0, 40);break;
	case 7: c_pos = ship_pos - Vector(0, 60, 60);break;
	default: c_pos = ship_pos - Vector(0, -160, 80);
	}

	return gfx::Camera(c_pos, c_at, c_up, 100, 75);
}

void Game::player_death(object::ID id)
{
	pl_dead.insert(id);
}


bool inv_bool(bool& b)
{
	b = !b;
	return b;
}

void inc_mod(int& v, int m)
{
	++v;
	v %= m;
}

void set_in_range(float& f, float min, float max)
{
	if (f < min)
		f = min;
	else if (f > max)
		f = max;
}

void nonlin_add(int &v, int s)
{
	if (v < 10 && s < 0)
		return;
	if (v < 500)
		v += s/10;
	else
		v += s;
}

bool Game::process_events(Scheduler& s)
{
	namespace id = os::event::id;
	os::event::Event e;
	if (sys_ep.get_event(e)) {
		if (e.id >= id::Btn0 || e.val < 0.1)
			return false;

		switch (e.id) {
		case id::Escape: return true;
		case id::F1: cam = 1; rr->settings().hud_type = gfx::None_HUD; break;
		case id::F2: cam = 2; rr->settings().hud_type = gfx::External_HUD; break;
		case id::F3: cam = 3; rr->settings().hud_type = gfx::Internal_HUD; break;
		case id::F4: cam = 4; rr->settings().hud_type = gfx::None_HUD; break;
		case id::F5: cam = 5; rr->settings().hud_type = gfx::None_HUD; break;
		case id::F6: cam = 6; rr->settings().hud_type = gfx::None_HUD; break;
		case id::F7: cam = 7; rr->settings().hud_type = gfx::None_HUD; break;
		case id::F8: cam = 8; rr->settings().hud_type = gfx::None_HUD; break;
		case '6': inc_mod(rr->settings().texture_level, 3);
			  break;
		case '7': inv_bool(rr->settings().use_lighting);
 			  break;
		case '8': inv_bool(rr->settings().draw_sky);
			  break;
		case '9': inv_bool(rr->settings().draw_terrain);
			  break;
		case '0': inv_bool(rr->settings().use_fog);
			  break;
		case 'Y': (++rr->settings().shadow_type) %= 6;
			  break;
		case 'U': inv_bool(rr->settings().draw_effects);
			  break;
		case 'I': rr->screenshot();
			  break;

		case 'W': { 	static int n = 1;
				conf.draw_timers = (n != 0);
				rr->settings().draw_stats = n-1;
				++n %= 6;
			}
			break;

		case 'R': {	static int r = 5;
				rr->settings().radar_range = 100 * (1 << r); 
				++r &= 7;
			}
			break;
		case 'S': rr->settings().terrain_detail -= 0.02;
		case 'X': rr->settings().terrain_detail += 0.01;
			  set_in_range(rr->settings().terrain_detail, 0.05, 1);
			  derr << "detail: " << rr->settings().terrain_detail << '\n';
			  break;
	  	case 'D': rr->settings().texture_scaling <<= 1;
			  if (rr->settings().texture_scaling > 16)
				  rr->settings().texture_scaling = 1;
			  rr->purge_textures();
			  derr << "texture scaled down to 1/" << rr->settings().texture_scaling << " th\n";
			  break;
		case 'F': 
			inv_bool(draw_fps);
			fps.reinit();
			break;
		case 'T':
		case 'G':
			nonlin_add(timescale, (e.id == 'T') ? 50 : -50);
			derr << "Time scaled by: " << std::fixed << std::setw(5)
			     << std::setprecision(3) << timescale / 1000.0 << '\n';
			break;
		case 'P': inv_bool(pause); loop_ticker.pause(pause); break;
		case id::F11: {
			  break;
			}
		case id::F12:
			return true;
		default: break;
		}
	}
	return false;
}




void Game::shutdown()
{
	derr << "Shutdown game\n";
	game.shutdown();
	if (sr.valid())
		sr->shutdown();
	pr->shutdown();
	wr->shutdown();
	if (sm.valid())
		sm->shutdown();
	derr << "objects after game::shutdown: " << object::get_obj_count() << '\n';
}


}
}

