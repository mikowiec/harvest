#ifndef REAPER_OBJECT_AI_H
#define REAPER_OBJECT_AI_H

#include "main/types.h"

#include "object/forward.h"
#include "object/baseptr.h"
#include "object/mkinfo.h"
#include "world/worldref.h"
#include "ai/fsm.h"
#include "ai/part.h"
#include "ai/msg.h"
#include <list>
#include <cmath>
#include <memory>
#include "main/enums.h"
#include "misc/command.h"

#include "ai/part.h"

using namespace reaper::ai;
using namespace reaper::ai::fsm;

namespace reaper {
namespace res { class ConfigEnv; }
namespace object {

namespace ai {

class Turret : public reaper::ai::Part
{
	MsgQueue messages;

	enum TurretEvent{EVENT_OBJECT_DETECTED, EVENT_TARGET_LOST};
	enum TurretState{IDLE=1, ATTACKING};
	
	FSM* fsm;	
	State* state[2];

	const object::SillyData &data;
	controls::Turret &tc;
	const current_data::Turret &current;
	Point pos;

	world::WorldRef wr; // referens to the world module

	Vector barrel_vec;
	float laser_speed;

	// Perform actions
	void idle(void);
	void attack(void);

public:
	Turret(const object::SillyData &d, controls::Turret &tctrl, 
		const current_data::Turret &cctrl, const Vector& barrel_vec);
	~Turret(void);

	// Executes the ai-algorithms
	void think();
};

class WayPointy
{
public:
	virtual void add_waypoint(const Point& p) = 0;
};

class ShipBase : public WayPointy, public reaper::ai::Part
{
protected:
	world::WorldRef wr; // referens to the world module
	MsgQueue messages;

	enum ShipEvent{EVENT_ENEMY_DETECTED, EVENT_ENEMY_KILLED, EVENT_TARGET_LOST,
		       EVENT_NEW_ATTACK, EVENT_IN_POSITION, EVENT_RETURN};
	enum ShipState{PATROLING=1, ATTACKING, EVADING};
	
	FSM* fsm;
	State* state[3];

	CompanyID company;
	object::GetDyn* dyn;

	controls::Ship &sc;

	std::list<Point> waypoints;        // list of waypoints if ship is patroling
	std::list<Point>::iterator wp_it;  // iterator pointing to current way point
	Point ep;			   // evade point

	misc::Ping rm_target_notify;
	object::SillyWPtr target_ptr; // pointer to data of current target
	bool new_attack;
	
	// Sets the yaw an pitch calculated from current velocity and desired direction
	// returns a messure of the differens between vel and dir (needed for aiming) 
	virtual float navigate(const Vector vel, const Vector dir, controls::Ship& sc) = 0;
	
	// Perform actions
	virtual void patrol(void) = 0;
	virtual void attack(void) = 0;
	virtual void evade(void) = 0;
	
	void target_death();
public:
	ShipBase(CompanyID cid, object::GetDyn* d, controls::Ship &sctrl);
	virtual ~ShipBase(void);

	// Executes the ai-algorithms
	void think();

	// Sends a message to this ship
//	void receive(const Message &m);

	// Adds a waypoints to the ships waypoint list, p.y becomes the height
	// over the terrain to make sure that no waypoint is put "inside" the terrain
	void add_waypoint(const Point& p);

	// Deletes a waypoint from the list
//	void del_waypoint(Point& p);
};

class ShipBomber : public ShipBase
{
	float navigate(const Vector vel, const Vector dir, controls::Ship& sc);
	void patrol(void);
	void attack(void);
	void evade(void);
	
public:
	ShipBomber(CompanyID cid, object::GetDyn* d, controls::Ship &sctrl);
	~ShipBomber(void);
};

class ShipFighter : public ShipBase
{
	int count;

	float navigate(const Vector vel, const Vector dir, controls::Ship& sc);
	void patrol(void);
	void attack(void);
	void evade(void);
	
public:
	ShipFighter(CompanyID cid, object::GetDyn* d, controls::Ship &sctrl);
	~ShipFighter(void);
};

class GVBase : public WayPointy, public reaper::ai::Part
{
protected:
	world::WorldRef wr;
	MsgQueue messages;

	CompanyID company;
	object::GetDyn* dyn;

	controls::GroundShip &sc;
					
	std::list<Point> waypoints;        
	std::list<Point>::iterator wp_it;

	void navigate(const Vector vel, const Vector dir, controls::GroundShip &sc);
	
public:
	GVBase(MkInfo, CompanyID cid, object::GetDyn* d, controls::GroundShip &sctrl);
//	void receive(Message m);
	void add_waypoint(const Point& p);
	void erase_waypoints(void);

	virtual ~GVBase(void);
	virtual void think() = 0;
};

class GVOrdinary : public GVBase
{
public:
	GVOrdinary(MkInfo, CompanyID cid, object::GetDyn* d, controls::GroundShip &sctrl);
	~GVOrdinary(void);
	void think(void);		
};

class GVContainer : public GVBase
{
	bool dest_reached;
public:
	GVContainer(MkInfo, CompanyID cid, object::GetDyn* d, controls::GroundShip &sctrl);
	~GVContainer(void);
	void think(void);		
};

class GVTurret : public GVBase
{	
	enum GVTurretEvent{EVENT_OBJECT_DETECTED, EVENT_TARGET_LOST};
	enum GVTurretState{PATROLING=1, ATTACKING};
	FSM* fsm;
	State* state[2];

	controls::Turret &tc;
	const current_data::Turret &current;

	void patrol(void);
	void attack(void);
	
public:
	GVTurret(MkInfo, CompanyID cid, object::GetDyn* d, controls::GroundShip &sctrl, 
		controls::Turret &tctrl, const current_data::Turret &cctrl);
	~GVTurret(void);
	void think(void);		
};


class DummyTurret
{
	controls::Turret &tc;

public:
	DummyTurret(controls::Turret &c) : tc(c) {}
	void think();
//	void receive(Message m) {}
};

}
}
}
#endif

/*
 * $Author: pstrand $
 * $Date: 2003/06/13 14:21:01 $
 * $Log: ai.h,v $
 * Revision 1.40  2003/06/13 14:21:01  pstrand
 * move code from quadtree.h to quadtree.cpp, and gvs thinking
 *
 * Revision 1.39  2003/06/04 17:43:05  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.38  2002/03/24 17:38:43  pstrand
 * turretss aims a bit bett
 *
 * Revision 1.37  2001/11/26 10:46:05  peter
 * kompilatorgnäll... ;)
 *
 * Revision 1.36  2001/11/11 01:23:11  peter
 * minnesfixar..
 *
 * Revision 1.35  2001/10/16 21:11:11  peter
 * waypoints..
 *
 * Revision 1.34  2001/10/03 12:54:35  peter
 * no message
 *
 * Revision 1.33  2001/08/27 12:55:27  peter
 * objektmeck...
 *
 * Revision 1.32  2001/08/20 17:15:43  peter
 * duh
 *
 * Revision 1.31  2001/08/20 17:09:56  peter
 * obj.ptr & fix...
 *
 * Revision 1.30  2001/08/06 12:16:35  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.29.2.2  2001/08/05 14:01:29  peter
 * objektmeck...
 *
 * Revision 1.29.2.1  2001/08/03 13:44:10  peter
 * pragma once obsolete...
 *
 * Revision 1.29  2001/07/30 23:43:25  macke
 * Häpp, då var det kört.
 *
 * Revision 1.28  2001/07/06 01:47:30  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.27  2001/05/10 10:15:08  niklas
 * egen ai till GroundTurret
 *
 * Revision 1.26  2001/05/10 02:04:55  niklas
 * Två olika skepp-ai
 *
 * Revision 1.25  2001/05/09 21:05:31  niklas
 * ai till GroundOrdinary
 *
 * Revision 1.24  2001/05/09 00:32:11  niklas
 * Betydligt bättre skepp-AI. Hastighetsanpassning, bättre terräng-undvikning,
 * lösning på "ringdansproblemet" m.m.
 *
 */

