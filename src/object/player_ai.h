#ifndef REAPER_OBJECT_PLAYER_AI_H
#define REAPER_OBJECT_PLAYER_AI_H

#include "object/forward.h"
#include "ai/part.h"
#include <memory>

namespace reaper {
namespace os { namespace event { class EventProxy; } }
namespace object {
namespace ai {

class PlayerControl : public reaper::ai::Part
{
	controls::Ship &ctrl;
	std::auto_ptr<os::event::EventProxy> ep;
public:
	PlayerControl(controls::Ship &c, int id);
	void think();
//	void add_waypoint(const Point&) { }
//	void del_waypoint(Point&)	{ }
//	void receive(const reaper::ai::Message&) { }
};

}
}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2003/06/04 17:43:07 $
 * $Log: player_ai.h,v $
 * Revision 1.7  2003/06/04 17:43:07  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.6  2003/01/06 12:42:11  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.5  2002/02/20 14:21:23  peter
 * eventhandling cleanup and networking fixes..
 *
 * Revision 1.4  2001/11/26 10:46:06  peter
 * kompilatorgnäll... ;)
 *
 * Revision 1.3  2001/08/27 12:55:30  peter
 * objektmeck...
 *
 * Revision 1.2  2001/08/06 12:16:39  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.1.4.4  2001/08/05 14:01:33  peter
 * objektmeck...
 *
 * Revision 1.1.4.3  2001/08/04 16:14:18  peter
 * *** empty log message ***
 *
 * Revision 1.1.4.2  2001/08/03 13:44:12  peter
 * pragma once obsolete...
 *
 * Revision 1.1.4.1  2001/08/03 13:37:12  peter
 * new weapon...
 *
 * Revision 1.1  2001/07/06 01:47:33  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 */
