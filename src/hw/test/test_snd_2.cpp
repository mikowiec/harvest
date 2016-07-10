
/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:06 $
 * $Log: test_snd_2.cpp,v $
 * Revision 1.4  2003/01/06 12:42:06  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.3  2002/12/29 16:59:28  pstrand
 * hw/ moved to os/
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

#include <iostream>
#include "misc/test_main.h"

using namespace reaper;
using namespace reaper::os::snd;


int test_main()
{
	os::gfx::Gfx gx;
	gx->change_mode(os::gfx::VideoMode(200, 100));
	os::event::EventSystem es(gx);
	os::event::EventProxy ep = os::event::EventSystem::get_ref(0);
	SoundSystem ss;
	ss.init();
	Music* mp3 = ss.prepare_music("reaper");
	Effect* eff1 = ss.prepare_effect("missile");
	Effect* eff2 = ss.prepare_effect("explosion_large");
	eff2->set_loop();
	//mp3->play();

	while (!exit_now()) {
		if (ep.key(os::event::id::Escape))
			break;
		if (ep.key('J'))
			eff1->play();
		if (ep.key('S'))
			eff2->play();
		if (ep.key('X'))
			eff2->stop();
		if (ep.key('M'))
			eff1->stop();
		ss.do_stuff();
		os::time::msleep(10);
		gx->update_screen();
	}
	mp3->stop();
	return 0;
}

