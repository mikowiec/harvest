/*
 * $Author: pstrand $
 * $Date: 2003/06/11 15:15:17 $
 * $Log: gvturret.cpp,v $
 * Revision 1.14  2003/06/11 15:15:17  pstrand
 * småfixar..
 *
 * Revision 1.13  2003/06/04 16:21:24  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.12  2002/12/29 17:04:14  pstrand
 * hw/ moved to os/
 *
 * Revision 1.11  2002/05/21 10:03:14  pstrand
 * world-iter.
 *
 * Revision 1.10  2002/01/31 04:55:52  peter
 * no message
 *
 * Revision 1.9  2001/12/03 18:06:14  peter
 * object/base.h-meck...
 *
 * Revision 1.8  2001/11/27 00:54:51  peter
 * worlditeratorer lämnar inte ifrån sig döda objekt längre..
 *
 * Revision 1.7  2001/08/06 12:16:01  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.6.2.1  2001/08/05 14:01:22  peter
 * objektmeck...
 *
 * Revision 1.6  2001/07/30 23:43:11  macke
 * Häpp, då var det kört.
 *
 * Revision 1.5  2001/05/14 00:26:42  niklas
 * högre hastighet
 *
 * Revision 1.4  2001/05/13 17:22:23  niklas
 * städ
 *
 * Revision 1.3  2001/05/12 10:00:14  niklas
 * småfix
 *
 * Revision 1.2  2001/05/10 10:14:31  niklas
 * egen ai till GroundTurret
 *
 * Revision 1.1  2001/05/06 14:18:57  niklas
 * gvs uppdelade på containers och turrets, gvturret är inte klar än...
 *
*/

#include "os/compat.h"
#include "object/ai.h"
#include "object/controls.h"
#include "object/base_data.h"
#include "object/mkinfo.h"
#include "main/types_ops.h"
#include "world/world.h"
#include "object/current_data.h"
#include "object/accessor.h"

namespace reaper{
namespace object{
namespace ai{

	enum GVTurretSpec
	{
		AIM_PRECISION = 5,
		DIST_WAYPOINT_REACHED = 50,
		VIEW_RANGE = 500
	};

	GVTurret::GVTurret(MkInfo mk, CompanyID cid, object::GetDyn* d, controls::GroundShip &sctrl, 
		controls::Turret &tctrl, const current_data::Turret &cctrl) 
		: GVBase(mk,cid,d,sctrl), tc(tctrl), current(cctrl)
	{
		state[0] = new fsm::State(PATROLING, 1);
		state[0]->add_transition(EVENT_OBJECT_DETECTED, ATTACKING);
		
		state[1] = new fsm::State(ATTACKING, 1);
		state[1]->add_transition(EVENT_TARGET_LOST, PATROLING);
		
		fsm = new fsm::FSM(PATROLING); // start at patroling
		fsm->add_state(state[0]);
		fsm->add_state(state[1]);

		wp_it = waypoints.end(); // no waypoints yet
	}

	GVTurret::~GVTurret()
	{	
		delete fsm;
	}

	void GVTurret::think()
	{
		switch(fsm->get_current_state())
		{
			case PATROLING : patrol(); break;
			case ATTACKING : attack(); break;
			default        : cout << "GVTurret, unknown state!" << endl;
		}
	}

	void GVTurret::patrol()
	{
		Point pos = dyn->pos();
		if(!waypoints.empty()){
			
			Point pwp = (*wp_it);   // Position of waypoint
			Vector dwp = pwp - pos; // Direction to waypoint
			
			// Check if waypoint reached
			if(length(dwp) < DIST_WAYPOINT_REACHED){
				wp_it++;
				if(wp_it == waypoints.end())
					wp_it = waypoints.begin();
				return;
			}

			// set default patrol thrust
			sc.thrust = 1.0;

			// set turn
			navigate(dyn->velocity(), dwp, sc);
		}

		world::Sphere view_sphere(pos, VIEW_RANGE);
		world::dyn_iterator i = wr->find_dyn(view_sphere);
		
		// if objects detected on radar 
		if(i != wr->end_dyn())
			fsm->state_transition(EVENT_OBJECT_DETECTED);
	}

	void GVTurret::attack()
	{
		Point pos = dyn->pos();
		// look for enemies and attack the closest one
		world::Sphere view_sphere(pos, VIEW_RANGE);
		world::dyn_iterator i = wr->find_dyn(view_sphere);
		
		Point pos_t;
		float min_dist = 10000000;
		bool enemy_found = false;
		
		for(; i != wr->end_dyn(); ++i){
			// if target alive and not a friend
			if ((*i)->stat().company() != company) {
				Point dp = (*i)->stat().pos();
				float dist = length(Vector(dp - pos));
				if(dist < min_dist){
					min_dist = dist;
					pos_t = dp; // remember this target
					enemy_found = true;
				}
			}
		}

		if(enemy_found){

			Vector aim_t = pos_t - pos; // aim vector

			tc.yaw = 57.296 * atan2(aim_t.x, aim_t.z) - 180;
			tc.pitch = 57.296 * atan2(aim_t.y, sqrt(aim_t.z*aim_t.z + aim_t.x*aim_t.x));
		
			// fire if aim is good
			if((fabs(current.yaw-tc.yaw)+fabs(current.pitch-tc.pitch)) < AIM_PRECISION){

				// make sure we don't hit or own buildings and turrets
				world::Line aim_line(pos, pos_t);
				world::si_iterator si_i = wr->find_si(aim_line);
				world::st_iterator st_i = wr->find_st(aim_line);
				if(si_i==wr->end_si() && st_i==wr->end_st())
					tc.fire = true;
			}
			else
				tc.fire = false;
			
			// move towards target
			// navigate(vel, aim_t, sc);
		}
		else{
			tc.fire = false;
			fsm->state_transition(EVENT_TARGET_LOST);
		}
	}
}
}
}
