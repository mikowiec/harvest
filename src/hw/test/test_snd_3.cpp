
/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:06 $
 * $Log: test_snd_3.cpp,v $
 * Revision 1.4  2003/01/06 12:42:06  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.3  2002/12/29 16:59:28  pstrand
 * hw/ moved to os/
 *
 * Revision 1.2  2001/08/20 16:59:40  peter
 * *** empty log message ***
 *
 * Revision 1.1  2001/07/27 15:47:37  peter
 * massive reorg...
 *
 * Revision 1.2  2001/04/18 16:25:47  henrik
 * lade till explosionsljud vid död...
 *
 * Revision 1.1  2001/04/11 14:44:01  peter
 * ljudfixar
 *
 *
 */

#include "os/compat.h"

#include "os/debug.h"
#include "os/snd.h"
#include "os/time.h"
#include "os/gfx.h"
#include "os/event.h"
#include "gfx/camera.h"
#include "os/worker.h"

#include <iostream>
#include "misc/test_main.h"

using namespace reaper;
using namespace reaper::os::snd;

namespace id = os::event::id;

class Foo : public os::worker::Job
{
	bool operator()() { return true; }
};

int test_main()
{
	os::gfx::Gfx gx;
	os::worker::worker()->add_job(new Foo);
	gx.change_mode(os::gfx::VideoMode(200, 100));
	os::event::EventSystem es(gx);
	os::event::EventProxy ep = os::event::EventSystem::get_ref(0);
	SoundSystem ss;
	ss.init();

	Effect* eff1 = ss.prepare_effect("missile");
	Effect* eff2 = ss.prepare_effect("explosion_large");
	eff2->set_loop();

	Camera cam(Point(0,0,0), Vector(0,0,-1), Vector(0,1,0), 90, 90);

	eff1->set_position(Point(-5, 0, 0));
	eff2->set_position(Point( 5, 0, 0));

	while (!exit_now()) {
		if (ep.key(os::event::id::Escape))
			break;
		if (ep.key('J'))
			eff1->play();
		if (ep.key('M'))
			eff1->stop();
		if (ep.key('S'))
			eff2->play();
		if (ep.key('X'))
			eff2->stop();

		if (ep.key(id::Left))
			cam.pos.x -= 10;
		if (ep.key(id::Right))
			cam.pos.x += 10;
		if (ep.key(id::Up))
			cam.pos.z -= 10;
		if (ep.key(id::Down))
			cam.pos.z += 10;

		ss.set_camera(cam);
		ss.do_stuff();
		os::time::msleep(100);
		gx.update_screen();
	}
	return 0;
}

