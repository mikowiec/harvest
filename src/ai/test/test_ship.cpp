#include "os/compat.h"
#include "gfx/gfx.h"
#include "os/gfx.h"
#include "os/event.h"
#include "os/time.h"
#include "os/font.h"
#include "world/world.h"
#include "gfx/main.h"
#include "gfx/object_manager.h"
#include "object/object.h"
#include "os/gl.h"
#include "os/debug.h"
#include "phys/engine.h"
#include <iostream>
#include <stdlib.h>

using namespace reaper;
using namespace reaper::gfx;
using namespace reaper::gfx::object;
using namespace reaper::hw;
using namespace reaper::os::gfx;
using namespace reaper::os::event;
using namespace reaper::os::time;
using namespace reaper::object;
using namespace reaper::phys;
using namespace std;

int main()
{      
        try {
		debug::DebugOutput dout("main",0);

		Gfx gx;
		gx->change_mode(VideoMode(320,200));

		world::WorldRef wr = world::World::create("test_level");              

                // Get a handle to the event-system
		EventSystem es(gx);
		EventProxy ep = EventSystem::get_ref(0);
                
                // Initialize graphics settings	
		Settings s;
                s.width = gx->current_mode().width;
                s.height = gx->current_mode().height;

                // Get interface to a renderer
		RendererRef rr = Renderer::create(s);
		ManagerRef mr = Manager::create();

		Point ship_pos(0,1000,0);
		Ship *ship = new Ship(SillyData(None,Matrix4(ship_pos)));
		wr->add_object(ship);
		
		ship->add_waypoint(Point(0,1000,-100));
		ship->add_waypoint(Point(-100,1000,-100));

		//Point p(0,500,0);
		//AiTestTurret *turret = new AiTestTurret(SillyData(None,Matrix4(p)),2);
		//wr->add_object(turret);
				
		Engine phys_en;

                timespan prevtime = get_time();
                
		// Loop
                while (true) {
                        static double time = 0;

			//if(time == 0)
			//	wr->add_object(new reaper::object::Laser(SillyData(None,Matrix4(Point(0,610,100))),0));

                        // Wait for event and exit when escape is pressed
                        if (ep.key(event::key::Escape))
                                break;

                        double timediff = static_cast<double>(get_time() - prevtime) * 1e-6;
                        prevtime = get_time();
						
			//turret->think();
			ship->think();

			phys_en.update_world(time,timediff);

			ship_pos = ship->get_pos();
			Camera c(ship_pos + Point(0,0,30), ship_pos, Vector(0,1,0), 90);

                        rr->render(c);
                        gx->update_screen();

                        if((time += timediff) > 50)
                                break;
                }

		dout << "Successful exit\n";
                return 0;
        }
        catch(const fatal_error_base &e) {
                cout << "Reaper fatal exception:" << e.what() << endl;
                return 1;
        } 
        catch(const error_base &e) {
                cout << "Reaper exception:" << e.what() << endl;
                return 1;
        }
        catch(exception &e) {
                cout << "Std exception: " << e.what() << endl;
                return 1;
        }
}
 
