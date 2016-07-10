#include "os/compat.h"

#include "os/gfx.h"
#include "os/event.h"
#include "os/time.h"
#include "misc/font.h"
#include "world/world.h"
#include "gfx/object_manager.h"
#include "object/object.h"
#include "os/gl.h"
#include "os/debug.h"
#include "phys/engine.h"
#include "gfx/gfx.h"
#include "gfx/camera.h"
#include <iostream>

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

		world::WorldRef wr = world::World::create("test_level");              

                // Get a handle to the event-system
		EventSystem es(gx);
		EventProxy ep = EventSystem::get_ref(0);
                
                // Get interface to a renderer
		RendererRef rr = Renderer::create(gx);
		ManagerRef mr = Manager::create();

		Ship *ship = new Ship(SillyData(reaper::None,Matrix4(Point(0,600,0))));

		wr->add_object(ship);

		for(int i=0; i<10; ++i) {
			Point p(i*8-10,595,-40);
			wr->add_object(new SillyTest(SillyData(reaper::None,Matrix4(p)),0,"ship"));
		}

		reaper::phys::Engine phys_en;

                timespan prevtime = get_time();
                // Loop
                while (true) {
                        static double time = 0;

			if(time == 0)
				wr->add_object(new reaper::object::Laser(SillyData(reaper::None,Matrix4(Point(0,610,100))),0));

                        // Wait for event and exit when escape is pressed
                        if (ep.key(event::id::Escape))
                                break;

                        double timediff = (get_time() - prevtime).val() * 1e-6;
                        prevtime = get_time();

			phys_en.update_world(time,timediff);

			Point p = ship->get_pos();	
	                Camera c(p + Point(2,10,20), p, Vector(0,1,0), 90, 90);

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
 
