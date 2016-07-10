#ifndef REAPER_OBJECT_CURRENT_DATA_H
#define REAPER_OBJECT_CURRENT_DATA_H

#include "misc/monitor.h"

namespace reaper {
namespace object {
namespace current_data {

class Engine {
public:
	misc::Monitored<float> thrust;
	bool afterburner;
	Engine() : thrust(0), afterburner(false) {}
};

class Turret {
public:
	float yaw;
	float pitch;
	Turret(float y, float p):
	yaw(y),pitch(p) {};
};

class Ship {
public:
	float roll;
	Engine eng;
	float afterburner_left;

	Ship() : roll(0.0),  afterburner_left(1.0) {}
};

class GroundShip : public Turret {
public:
        double accum_alt;
        Engine eng;
        GroundShip(float y, float p) : Turret(y,p),accum_alt(0.0) {}
        GroundShip() : Turret(0,0), accum_alt(0) {}
};
                                
}
}
}
#endif

/*
 * $Author: peter $
 * $Date: 2001/08/06 12:16:36 $
 * $Log: current_data.h,v $
 * Revision 1.5  2001/08/06 12:16:36  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.4.2.2  2001/08/05 14:01:30  peter
 * objektmeck...
 *
 * Revision 1.4.2.1  2001/08/03 13:44:10  peter
 * pragma once obsolete...
 *
 * Revision 1.4  2001/07/30 23:43:27  macke
 * Häpp, då var det kört.
 *
 * Revision 1.3  2001/07/06 01:47:31  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 */
