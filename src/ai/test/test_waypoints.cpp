#include "os/compat.h"
#include "gfx/gfx.h"
#include "os/gfx.h"
#include "os/event.h"
#include "os/time.h"
#include "os/mapping.h"
#include "world/world.h"
#include "gfx/misc.h"
#include "gfx/object.h"
#include "gfx/object_manager.h"
#include "object/object.h"
#include "os/gl.h"
#include "os/debug.h"
#include "phys/engine.h"
#include <iostream>
#include "net/game_mgr.h"
#include "snd/sound_system.h"
#include "misc/font.h"

using namespace reaper;

using object::SillyData;
using object::SillyTest;
using object::Ship;
using object::Turret;
using object::Sheep;

using os::time::timespan;

using os::event::EventProxy;
using os::event::EventSystem;
using net::PlayerID;

using phys::Engine;

using gfx::meter;

namespace id = os::event::id;

namespace { debug::DebugOutput derr("test_input"); }

class TestShip : public object::Ship
{
	PlayerID my_id;
public:

	TestShip(PlayerID id, const SillyData &d)
	 : Ship(d), my_id(id)
	{
		ctrl.thrust = .5 ;
	}
	void think() {
		//Ship::think();
		static EventProxy ep = EventSystem::get_ref(my_id);
		if (ep.key('A'))
			ctrl.thrust+=0.1;
		if (ep.key('Z'))
			ctrl.thrust-=0.1;
		if (ep.key(id::Left)) {
			ctrl.yaw+=0.1;
//			derr << "Left\n";
		}
		if (ep.key(id::Right)) {
			ctrl.yaw-=0.1;
//			derr << "Right\n";
		}
		if (ep.key(id::Down)) {
			ctrl.pitch+=0.1;
//			derr << "Down\n";
		}
		if (ep.key(id::Up)) {
			ctrl.pitch-=0.1;
//			derr << "Up\n";
		}
		if (ep.key(' '))
			ctrl.fire = true;
		else
			ctrl.fire = false;

		if (ep.axis_changed(1))
			ctrl.pitch = ep.axis(1);
		if (ep.axis_changed(0))
			ctrl.yaw = -ep.axis(0);

		if (ctrl.yaw > 1)  ctrl.yaw = 1;
		if (ctrl.yaw < -1) ctrl.yaw = -1;
		if (ctrl.pitch > 1)  ctrl.pitch = 1;
		if (ctrl.pitch < -1) ctrl.pitch = -1;
		if (ctrl.thrust > 1)  ctrl.thrust = 1;
		if (ctrl.thrust < 0) ctrl.thrust = 0;
	}
	void collide(object::CollisionInfo&) { }
};

using reaper::os::event::Event;

class esc_map
 : public reaper::os::event::Mapping
{
public:
	virtual Event operator()(const Event& ev)
	{
		if (ev.id < id::Axis0) {
			Event e(ev);
			if (e.id == id::Escape
			 || (e.id >= id::F1 && e.id <= id::F12)
			 || (e.id >= '0' && e.id <= '9')
			 || (e.id == 'S' || e.id == 'X'))
				e.recv = os::event::System;
			else {
				e.recv = os::event::Player;
			}
			if (e.id < 128)
				e.id = id::mk(toupper(e.id));
			return e;
		} else {
			Event e(ev);
			if (e.id >= id::Axis2)
				e.id = id::mk(e.id - 2);
			e.recv = os::event::Player;
			return e;
		}
	}
};


int main(int argc, char *argv[])
{      
	debug::DebugOutput dout("test_input",0);
	int frames = 0;
        double time = 0;

	int* i = new int;
        try {

		os::gfx::Gfx gx;

		dout << os::gfx::OpenGLinfo();

		world::WorldRef wr = world::World::create("test_level");
		net::GameMgr game;
		PlayerID id = game.init_singleplayer();

		sound::SoundManager snd;

                // Get a handle to the event-system
		EventSystem es(gx);
		EventProxy ep = EventSystem::get_ref(0);
		es.set_mapping(os::event::create_mapping(esc_map()));

                // Get interface to a renderer
		gfx::RendererRef rr = gfx::Renderer::create(gx);
/*
		rr->draw_dynamic_objects(false);
		rr->draw_static_objects(false);
		rr->draw_sky(false);
		rr->use_multitexturing(false);
		rr->use_fog(false);
*/
		for(int i = 1; i<argc; i++) {
			if(strcmp(argv[i],"-m") == 0) {
				rr->use_multitexturing(false);
			} else if(strcmp(argv[i],"-f") == 0) {
				rr->use_fog(false);
			} else {
				dout << "Unknown command line argument:" << argv[i] << "\n";
			}
		}			
				
		float tur_pos[11][3] = {
			{-2055.579 ,513.1, -2416.7},
			{-2172.5, 423, -2662.2},
			{-2114.7, 410.8, -2927.9},
			{-1977.3, 420.7, -3161.9},
			{-1663.6, 424.5, -3293.6},
			{-758.7, 309.3, -2689.4},
			{-140, 195.3, -2265.5},
			{0.7, 183.5, -2183.6},
			{405.4, 161.7, -2183.4},
			{856.3, 193.6, -1971.7},
			{1076.6, 217.5, -1679.9}
		};

		Turret* t;
		std::vector<Turret*> turrets;

		t = new Turret(SillyData(None,Matrix4(Point(tur_pos[3][0], tur_pos[3][1], tur_pos[3][2]))));
		wr->add_object(t);
		Turret* turret2 = t;
	 		
		t = new Turret(SillyData(None,Matrix4(Point(tur_pos[4][0], tur_pos[4][1], tur_pos[4][2]))));
		wr->add_object(t);
		Turret* turret = t;
	
/*		for (int i = 1; i < 11; i++) {
			Turret *obj = new Turret(SillyData(None,Matrix4(Point(tur_pos[i][0], tur_pos[i][1], tur_pos[i][2]))));		
			wr->add_object(obj);
		}
*/
//		Sheep* sheep = new Sheep(SillyData(None,Matrix4(Point(1000,0,-400))));
//		wr->add_object(sheep);

		Ship* ship = new TestShip(id, SillyData(Horny,Matrix4(Point(1005.1, 700.0, 0))));
		wr->add_object(ship);
		wr->set_local_player(ship);

/*		Ship *ship1 = new Ship(SillyData(None,Matrix4(Point(0,700,0))));
		wr->add_object(ship1);
		ship1->add_waypoint(Point(0,100,-200));
		//ship1->add_waypoint(Point(-2000,20,2000));
		ship1->add_waypoint(Point(2000,10,2000));
		

		Ship *ship2 = new Ship(SillyData(Horny,Matrix4(Point(2000,800,20))));
		wr->add_object(ship2);
		ship2->add_waypoint(Point(0,100,-2000));
		ship2->add_waypoint(Point(2000,800,-2000));
		//ship1->add_waypoint(Point(2000,850,0));
		//ship2->add_waypoint(Point(0,700,0));
*/
		// render to load everything
		gfx::Camera c(Point(0,2000,0),Point(0,0,0),Vector(1,0,0),90);
		rr->render(c);
                gx->update_screen();
		os::time::msleep(2000);

		reaper::phys::Engine phys_en;
                timespan prevtime = os::time::get_time();

		std::list<Point> poslist(20,ship->get_pos() - normalize3(ship->get_velocity()) * 15);

		os::time::Profiler phys("Phys"), gfx("Gfx "), ai("AI  "), wait("Wait"), io("IO  "), frame;

		frame.start();
                // Loop
                while (true) {
			static int cam = 0;
			static float td = 1;

			io.start();
                        // Wait for event and exit when escape is pressed
                        if (ep.key(id::Escape)) break;
                        if (ep.key(id::F1)) cam = 1;
                        if (ep.key(id::F2)) cam = 2;
                        if (ep.key(id::F3)) cam = 3;
                        if (ep.key(id::F4)) cam = 4;
                        if (ep.key(id::F5)) cam = 5;
                        if (ep.key(id::F6)) cam = 6;
                        if (ep.key(id::F7)) cam = 7;
                        if (ep.key(id::F8)) cam = 8;
	
			if (ep.key('1')) rr->use_texturing(false);
			if (ep.key('2')) rr->use_lighting(false);
			if (ep.key('3')) rr->draw_sky(false);
			if (ep.key('4')) rr->draw_terrain(false);
			if (ep.key('5')) rr->use_fog(false);

			if (ep.key('6')) rr->use_texturing(true);
			if (ep.key('7')) rr->use_lighting(true);
			if (ep.key('8')) rr->draw_sky(true);
			if (ep.key('9')) rr->draw_terrain(true);
			if (ep.key('0')) rr->use_fog(true);


			if (ep.key('S')) {
				td -= 0.01;
				if(td<0.05) td = 0.05;
				rr->terrain_detail(td);
			}				
			if (ep.key('X')) {
				td += 0.01;
				if(td>1) td = 1;
				rr->terrain_detail(td);
			}				

			ship->think();
			io.stop();

                        double timediff =  (os::time::get_time() - prevtime).val() * 1e-6;
                        prevtime = os::time::get_time();

			ai.start();
			turret->think();
			turret2->think();
			//ship1->think();
			//ship2->think();
			ai.stop();

			phys.start();
			phys_en.update_world(time,timediff);
			phys.stop();

			Point ct = ship->get_pos();
			Point e;
			switch (cam) {
			case 1:	 e = ship->get_pos() - ship->get_velocity()/5 - normalize3(ship->get_velocity()) * 15 + Point(0,5,0); break;
			case 2:  e = poslist.back(); break;
			case 3:  break;
			case 4:  e = turret->get_pos() + Vector(0, 20, 20); break;
			case 5:  e = turret2->get_pos() + Vector(0, 20, 20); break;
			case 6:	 e = ship->get_pos() + Vector(0,20, 20); ct = ship->get_pos(); break;
			case 7:  e = turret2->get_pos() + Vector(0, 100, 20); break;
			default: e = ship->get_pos() - Vector(0, -10, 10);
			}
			poslist.pop_back();
			poslist.push_front(ct);

			Vector v(0,1,0);

			if(cam==2) {
				static valarray<Vector> prev(v,60);
				static int cnt = 0;
				static float dt = 0;
				static int maxval = prev.size();
				ct = ship->get_pos() + normalize3(ship->get_velocity())*100;
				e = ship->get_pos() - normalize3(ship->get_pos()-e) * 15 *
					              (1+length3(ship->get_velocity())/75) + Vector(0, 2, 0);

				prev[cnt++] = ship->get_data().m.get_row(1);
				if(dt > .5 || cnt > prev.size()) {
					maxval = cnt;
					cnt = 0;
					dt = 0;
				}

				dt += timediff;

				Vector sum(0,0,0);
				for(int i=0;i<maxval;i++)
					sum += prev[i];

				v = ((sum / maxval) + Vector(0,1,0))/2;
			} else if(cam == 3) {
				e = ship->get_pos() - ship->get_data().m.get_row(2);
				ct = e - ship->get_data().m.get_row(2);
				v = normalize3(ship->get_data().m.get_row(1));
			}

/*			if(ship->get_data().is_dead()) {
				static float angle = 0;
				angle += timediff/3;
				ct = ship1->get_pos();
				e = ct + Vector(sin(angle)*50,20,cos(angle)*50);
			}
*/
			gfx::Camera c(e, ct, v,70);

			frame.stop();

			glPushAttrib(GL_ENABLE_BIT);      
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_LIGHTING);

			glColor3f(1,1,1);
			glPushMatrix();
			glLoadIdentity();
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(0,1,0,80);
			using gfx::font::glPutStr;

				glPutStr(0, 0, ai.report_last(), 0.008, 1.9);
				glPutStr(0, 2, phys.report_last(), 0.008, 1.9);
				glPutStr(0, 4, gfx.report_last(), 0.008, 1.9);
				glPutStr(0, 6, io.report_last(), 0.008, 1.9);
				glPutStr(0, 8, wait.report_last(), 0.008, 1.9);

				glMatrixMode(GL_MODELVIEW);
       
				glColor3f(0,0,1);
				meter(.5,0, .5, 1, .33, ai.get_last()/(float)frame.get_last());
				meter(.5,2, .5, 1, .33, phys.get_last()/(float)frame.get_last());
				meter(.5,4, .5, 1, .33, gfx.get_last()/(float)frame.get_last());
				meter(.5,6, .5, 1, .33, io.get_last()/(float)frame.get_last());
				meter(.5,8, .5, 1, 1, wait.get_last()/(float)frame.get_last());

				glColor3f(0,1,0);
				meter(.5,1, .5, 1, .33, ai.get_avg()/frame.get_avg());
				meter(.5,3, .5, 1, .33, phys.get_avg()/frame.get_avg());
				meter(.5,5, .5, 1, .33, gfx.get_avg()/frame.get_avg());
				meter(.5,7, .5, 1, .33, io.get_avg()/frame.get_avg());
				meter(.5,9, .5, 1, 1, wait.get_avg()/frame.get_avg());

			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glPopAttrib();

			frame.start();
			wait.start();
                        gx->update_screen();
			wait.stop();

			gfx.start();
                        rr->render(c);
			gfx.stop();

                        if((time += timediff) > 5000)
                                break;

			frames++;
                }

		dout << "Successful exit. Average FPS = " << frames/time << '\n';

		ai.report();
		phys.report();
		gfx.report();
		wait.report();
		io.report();
                return 0;
        }
        catch(const fatal_error_base &e) {
                dout << e.what() << endl;
                return 1;
        } 
        catch(const error_base &e) {
                dout << e.what() << endl;
                return 1;
        }
        catch(exception &e) {
                dout << "Std exception: " << e.what() << endl;
                return 1;
        }
}
 
