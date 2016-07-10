#ifndef REAPER_OBJECT_CONTROLS_H
#define REAPER_OBJECT_CONTROLS_H

#include "main/enums.h"
#include "misc/monitor.h"

namespace reaper {
namespace object {
namespace controls {

class Ship
{
public:
	float pitch;
	float yaw;
	float thrust;

//	bool fire;
//	bool missile_fire;
	misc::Switch fire;
	misc::Switch missile_fire;
	bool afterburner;

	misc::Monitored<MissileType> current_missile;

	Ship() :
		pitch(0), yaw(0), thrust(0.2),
		fire(false), missile_fire(false),afterburner(false), 
		current_missile(Light_Missile)
	{}

	void select_next_missile();
	void select_prev_missile();
	void dump(std::ostream& os) const;
};

class Turret
{
public:
	float pitch;
	float yaw;

	misc::Switch fire;

	Turret() : pitch(0), yaw(0), fire(false) {}
};

class GroundShip:
        public Turret
{
public:
        float turn;
        float thrust;
        GroundShip() :
                Turret(),turn(0.0),thrust(0.5) {}
};


}
}
}
#endif

/*
 * $Author: pstrand $
 * $Date: 2002/03/18 13:33:29 $
 * $Log: controls.h,v $
 * Revision 1.14  2002/03/18 13:33:29  pstrand
 * load&save
 *
 * Revision 1.13  2002/02/21 15:24:25  peter
 * *** empty log message ***
 *
 * Revision 1.12  2001/12/08 23:23:56  peter
 * 'spara/fixar/mm'
 *
 * Revision 1.11  2001/08/06 12:16:36  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.10.2.6  2001/08/05 14:01:30  peter
 * objektmeck...
 *
 * Revision 1.10.2.5  2001/08/04 20:37:07  peter
 * flyttad hud, vapenknôk..
 *
 * Revision 1.10.2.4  2001/08/04 16:14:16  peter
 * *** empty log message ***
 *
 * Revision 1.10.2.3  2001/08/03 13:44:10  peter
 * pragma once obsolete...
 *
 * Revision 1.10.2.2  2001/08/03 13:37:11  peter
 * new weapon...
 *
 * Revision 1.10.2.1  2001/08/02 16:29:11  peter
 * objfix...
 *
 * Revision 1.10  2001/07/30 23:43:27  macke
 * Häpp, då var det kört.
 *
 * Revision 1.9  2001/07/06 01:47:30  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.8  2001/05/25 09:54:46  peter
 * ljudfixar..
 *
 * Revision 1.7  2001/05/10 11:40:22  macke
 * häpp
 *
 * Revision 1.6  2001/05/10 09:49:04  peter
 * fylla...
 *
 * Revision 1.5  2001/04/29 19:59:49  picon
 * Adderar en ground vehicle
 *
 * Revision 1.4  2001/01/21 22:52:45  peter
 * ..
 *
 * Revision 1.3  2001/01/21 00:22:40  peter
 * initordning
 *
 * Revision 1.2  2001/01/18 10:18:27  picon
 * adderade afterburner
 *
 * Revision 1.1  2001/01/16 00:46:28  macke
 * objects improved
 *
 */

