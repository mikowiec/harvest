
#include "os/compat.h"

#include "os/gfx.h"
#include "os/event.h"
#include "os/time.h"

#include "os/gl.h"
#include "os/debug.h"

#include "gfx/gfx.h"
#include "gfx/pm/pm.h"

#include "world/world.h"

using namespace reaper;


using os::time::TimeSpan;

using os::event::EventProxy;
using os::event::EventSystem;


namespace id = os::event::id;

namespace { debug::DebugOutput derr("test_input"); }

using reaper::os::event::Event;


int main(int argc, char *argv[])
{      
	debug::DebugOutput dout;
	int frames = 0;
        double time = 0;

        try {

		os::gfx::Gfx gx;

		//gx->change_mode(os::gfx::VideoMode(1280, 1024));


		dout << os::gfx::OpenGLinfo();

		world::WorldRef wr = world::World::create("test_level3");
		//net::GameMgr game(gx);
		// PlayerID id = game.init_singleplayer();

//		sound::SoundManager snd;

        // Get a handle to the event-system
		EventSystem es(gx);
		EventProxy ep = EventSystem::get_ref(os::event::System);
//		es.set_mapping(os::event::create_mapping(esc_map()));



        // Get interface to a renderer
		gfx::RendererRef rr = gfx::Renderer::create(&gx);

		// Prepare a pointer to the pm
		gfx::pm::Pmd * pm = rr->get_pmd();
		


		for(int i = 1; i<argc; i++) {
			if(strcmp(argv[i],"-m") == 0) {
//				rr->use_arb_multitexturing(false);
			} else if(strcmp(argv[i],"-f") == 0) {
				rr->use_fog(false);
			} else {
				dout << "Unknown command line argument:" << argv[i] << "\n";
			}
		}			

		// Use FlatShading for better polygon distinction, and no culling...just in case
		::glDisable(GL_CULL_FACE);
		//glShadeModel(GL_FLAT);

		
		// render to load everything
		gfx::OldCamera c(Point(0,0,0),Point(0,0,1),Vector(0,1,0),90, 90);

		rr->render(c, 0);
        gx.update_screen();

		// I will need a testcamera
		gfx::OldCamera tc(Point(0,0,0), Point(0,0, 1000), Vector(0,1,0), 120, 120);
		Point cv1, cv2;
		Vector cv3;

//		reaper::phys::Engine phys_en;
        TimeSpan prevtime = os::time::get_time();

		//std::list<Point> poslist(20,ship->get_pos() - normalize_3(ship->get_velocity()) * 15);

		os::time::Profiler phys("Phys"), gfx("Gfx "), ai("AI  "), wait("Wait"), io("IO  "), frame;


		frame.start();
        
		
		// Main Loop

		Point c_origin(0,2000,0);
		Vector c_up(1, 0, 0);
		double alpha = 0.0;
		double delta_alpha = 0.0;
        
		rr->use_fog(false);
		rr->draw_sky(false);

		pm->set_num_con_lim(20000.0);

		double alpha_threshold = 1.5E-9;
		pm->set_alpha_threshold(alpha_threshold);
		pm->set_update_steps(10);

		while (true) {

			//Rotate the camera


			c_up.x = cos(alpha);
			c_up.z = sin(alpha);
			alpha += delta_alpha;

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

			double tc_d = 100;

			if (ep.key(id::Left)){
				tc.eye.z-=tc_d;
				tc.center.z-=tc_d;
			}
	
			if (ep.key(id::Right)){
				tc.eye.z+=tc_d;
				tc.center.z+=tc_d;
			}
			if (ep.key(id::Down)){
				tc.eye.x-=tc_d;
				tc.center.x-=tc_d;
			}
	
			if (ep.key(id::Up)){
				tc.eye.x+=tc_d;
				tc.center.x+=tc_d;
			}

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

			if (ep.key('U')) {c_origin.y+=200; tc.eye.y+=200;}
			if (ep.key('J')) {c_origin.y-=200; tc.eye.y-=200;}

			if (ep.key('I')) delta_alpha += 0.01;
			if (ep.key('K')) delta_alpha -= 0.01;

			if (ep.key('O')){
				cv1 = tc.eye;
				cv2 = tc.center;
				Vector tmp = cv2 - cv1;
				cv3 = tmp;
				float ang = 0.05;
				
				cv3.x = tmp.x * cos(ang) - tmp.z * sin(ang);
				cv3.z = tmp.x * sin(ang) + tmp.z * cos(ang);
				tc.center = (cv3 / length(cv3)).to_pt();
				tc.center = tc.center * 3000;
				tc.center += tc.eye;
			}


			if (ep.key('L')){
				cv1 = tc.eye;
				cv2 = tc.center;
				Vector tmp = cv2 - cv1;
				cv3 = tmp;
				float ang = -0.05;
				
				cv3.x = tmp.x * cos(ang) - tmp.z * sin(ang);
				cv3.z = tmp.x * sin(ang) + tmp.z * cos(ang);
				tc.center = (cv3 / length(cv3)).to_pt();
				tc.center = tc.center * 3000;
				tc.center += tc.eye;
			}


			if (ep.key('S')) {
				td -= 0.01;
				if(td<0.00) td = 0.00;
				rr->terrain_detail(td);
			}				
			if (ep.key('X')) {
				td += 0.01;
				if(td>1) td = 1;
				rr->terrain_detail(td);
			}				
			if (ep.key('A')) {
				pm->analyze_contractions();
			}				

			if (ep.key('Q')) {
				pm->set_alpha_threshold(alpha_threshold-=1E-10);
				cout << "alpha_threshold: " << alpha_threshold << endl;
			}				
			
			if (ep.key('W')) {
				pm->set_alpha_threshold(alpha_threshold+=1E-10);
				cout << "alpha_threshold: " << alpha_threshold << endl;
			}				

			//rr->terrain->render(c);
		    
			//ship->think();
			io.stop();

            double timediff = (os::time::get_time() - prevtime).approx().to_s();
            prevtime = os::time::get_time();

			/*
			ai.start();
			turret->think();
			turret2->think();
			ai.stop();
			*/

//			phys.start();
//			phys_en.update_world(time,timediff);
//			phys.stop();
		
			// Fjolleri. Nu vill jag ha overview
			
			Point c_lookat(0, 0, 0);
			
			gfx::OldCamera c(c_origin, c_lookat, c_up, 120, 120);

			//pm = rr->get_pmd();

			
			//dout << "setcam" << endl;
			//pm->set_cam(tc);
			//cin.get();
			
			
			//dout << "update";
			//pm->update_fast();
			//pm->update();
			//cin.get();
			

			//dout << "report" << endl;
			//pm->report();
			//cin.get();
			

			gfx.start();
                        rr->render(tc, timediff);
						//dout << "Active Faces: " << pm->active_
			gfx.stop();
			frame.stop();

			//pm->report_config();

			// Userviewport bounding-lines

			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			
			cv1 = tc.eye;
			cv2 = tc.center;
			Vector tmp = cv2 - cv1;
			cv3 = tmp;

			
			float angle = (tc.horiz_fov*3.14159/180.0) / 2;
			cv3.x = tmp.x * cos(angle) - tmp.z * sin(angle);
			cv3.z = tmp.x * sin(angle) + tmp.z * cos(angle);
			cv3 = cv3 / length(cv3);
			cv3 = cv3 * 3000;
			cv3 += cv1;

			cv2.x = tmp.x * cos(-angle) - tmp.z * sin(-angle);
			cv2.z = tmp.x * sin(-angle) + tmp.z * cos(-angle);
			cv2 = cv2 / length(cv2);
			cv2 = cv2 * 3000;
			cv2 += cv1;

			::glDisable(GL_DEPTH_TEST);
			glBegin(GL_LINES);

				glColor3f(1.0, 0.0, 0.0);
				
				glVertex3fv(cv1.get());
				glVertex3fv(cv3.get());

				glVertex3fv(cv1.get());
				glVertex3fv(cv2.get());

			glEnd();
			::glEnable(GL_DEPTH_TEST);


			glPopMatrix();
/*
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

		
				ai.report_last(0,0.0);
				phys.report_last(0,2);
				gfx.report_last(0,4);
				io.report_last(0,6);
				wait.report_last(0,8);
			
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
*/
			frame.start();
			wait.start();
                        gx.update_screen();
			wait.stop();

			if((time += timediff) > 5000)break;

			frames++;
        }

		dout << "Successful exit. Average FPS = " << frames/time << '\n';

		gfx.report();
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
 
