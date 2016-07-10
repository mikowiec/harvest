/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:40:58 $
 * $Log: player.cpp,v $
 * Revision 1.12  2003/01/06 12:40:58  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.11  2002/12/29 17:04:14  pstrand
 * hw/ moved to os/
 *
 * Revision 1.10  2002/03/24 17:31:57  pstrand
 * turret aims better, and uses correct (barrel and laser) information
 *
 * Revision 1.9  2002/03/11 10:50:38  pstrand
 * hw/time.h -> reltime.h&abstime.h, and some more..
 *
 * Revision 1.8  2002/02/21 15:29:35  peter
 * *** empty log message ***
 *
 * Revision 1.7  2002/02/21 15:29:25  peter
 * *** empty log message ***
 *
 * Revision 1.6  2002/02/20 14:21:19  peter
 * eventhandling cleanup and networking fixes..
 *
 * Revision 1.5  2002/01/31 05:34:25  peter
 * *** empty log message ***
 *
 * Revision 1.4  2002/01/01 23:29:50  peter
 * alloc_id
 *
 * Revision 1.3  2001/08/27 12:55:23  peter
 * objektmeck...
 *
 * Revision 1.2  2001/08/06 12:16:02  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.1.2.1  2001/08/05 14:01:23  peter
 * objektmeck...
 *
 *
*/

#include "os/compat.h"
#include "main/types.h"
#include "object/player_ai.h"
#include "object/controls.h"
#include "world/world.h"
#include "msg/event.h"
#include "os/debug.h"

#include <algorithm>
#include <iomanip>
#include <stdio.h>

namespace reaper {
namespace object {
namespace ai {


using namespace reaper::os::event;

PlayerControl::PlayerControl(controls::Ship& c, int id)
 : ctrl(c), ep(EventSystem::create_ref(id))
{ }

void PlayerControl::think()
{
	os::event::Event e;
	while (ep->get_event(e)) {
		bool dn = e.is_pressed();
		switch (e.id) {
			case ' ': ctrl.fire = dn;         break;
			case 'M': ctrl.missile_fire = dn; break;
			case 'Q': ctrl.afterburner = dn;  break;
			case 'N': if (dn) ctrl.select_next_missile(); break;
			case 'B': if (dn) ctrl.select_prev_missile(); break;
			case id::Axis0: ctrl.yaw = e.val;    break;
			case id::Axis1: ctrl.pitch = e.val;  break;
			case id::Axis3: ctrl.thrust = std::max(-1.0f,static_cast<float>(e.val))	; break;
			default: break;
		}
	}
}

}
}
}
