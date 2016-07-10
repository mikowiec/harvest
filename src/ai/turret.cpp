/*
 * $Author: pstrand $
 * $Date: 2003/06/13 14:19:38 $
 * $Log: turret.cpp,v $
 * Revision 1.39  2003/06/13 14:19:38  pstrand
 * move code from quadtree.h to quadtree.cpp, and gvs thinking
 *
 * Revision 1.38  2003/06/11 15:15:18  pstrand
 * småfixar..
 *
 * Revision 1.37  2003/06/04 16:21:24  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.36  2002/12/29 17:04:15  pstrand
 * hw/ moved to os/
 *
 * Revision 1.35  2002/05/21 10:03:15  pstrand
 * world-iter.
 *
 * Revision 1.34  2002/03/24 17:31:57  pstrand
 * turret aims better, and uses correct (barrel and laser) information
 *
 * Revision 1.33  2002/01/31 04:55:51  peter
 * no message
 *
 * Revision 1.32  2001/12/03 18:06:15  peter
 * object/base.h-meck...
 *
 * Revision 1.31  2001/11/27 00:54:51  peter
 * worlditeratorer lämnar inte ifrån sig döda objekt längre..
 *
 * Revision 1.30  2001/10/09 01:12:23  macke
 * Nu siktar den rätt.. (fulhack)
 *
 * Revision 1.29  2001/08/27 12:55:23  peter
 * objektmeck...
 *
 * Revision 1.28  2001/08/09 18:11:39  macke
 * Fyllefix.. lite här å där..
 *
 * Revision 1.27  2001/08/06 12:16:03  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.26  2001/08/01 21:47:37  macke
 * Lite mer våldsamt.. :)
 *
 *
 */


#include <math.h>
#include "os/compat.h"
#include "os/debug.h"
#include "object/ai.h"
#include "object/base_data.h"
#include "object/controls.h"
#include "object/factory.h"
#include "object/current_data.h"
#include "object/accessor.h"
#include "world/world.h"
#include "main/types_io.h"
#include "main/types_ops.h"

namespace reaper
{
namespace object
{
namespace ai
{
	enum TurretSpec
	{
		RADIUS = 15, // Ändra eventuellt
		AIM_PRECISION =  5,
		VIEW_RANGE = 1000
	};

	Turret::Turret(const object::SillyData &d, controls::Turret &tctrl, const current_data::Turret &cctrl, const Vector& bv)
		: data(d), tc(tctrl), current(cctrl), pos(d.get_pos()), barrel_vec(bv),
		  laser_speed(object::factory::inst().info("laser")["speed"])
	{
		state[0] = new fsm::State(IDLE, 1);
		state[0]->add_transition(EVENT_OBJECT_DETECTED, ATTACKING);
		
		state[1] = new fsm::State(ATTACKING, 1);
		state[1]->add_transition(EVENT_TARGET_LOST, IDLE);
		
		fsm = new fsm::FSM(IDLE); // start at idle
		fsm->add_state(state[0]);
		fsm->add_state(state[1]);
	}

	Turret::~Turret()
	{
		delete fsm; // FSM destructor also deallocates all states inside
	}

	void Turret::think()
	{
		switch(fsm->get_current_state())
		{
			case IDLE      : idle();   break;
			case ATTACKING : attack(); break;
			default        : cout << "Turret, unknown state!!!" << endl;
		}
	}

	void Turret::idle(void)
	{
		//cout << "Turret state: IDLE \n";
		world::Sphere view_sphere(pos, VIEW_RANGE);
		world::dyn_iterator i = wr->find_dyn(view_sphere);
		
		// if objects detected on radar 
		if(i != wr->end_dyn())
			fsm->state_transition(EVENT_OBJECT_DETECTED);	
		
	}

	void Turret::attack(void)
	{						
		// look for enemies and attack the closest one
		world::Sphere view_sphere(pos, VIEW_RANGE);
		world::dyn_iterator i = wr->find_dyn(view_sphere);
		
		Point pos_t;
		Vector vel_t;
		float min_dist = 10000000;
		bool enemy_found = false;
		
		for(; i != wr->end_dyn(); ++i){
			// if target alive and not a friend
			if (((*i)->stat().company() != data.get_company())
			 && ((*i)->stat().company() != Nature)) {

				Point dp = (*i)->stat().pos();
				float dist = length(Vector(dp - pos));
				if(dist < min_dist){
					min_dist = dist;	
					pos_t = dp; // remember this target
					vel_t = (*i)->dyn().velocity();
					enemy_found = true;
				}
			}
		}

		if(enemy_found){

			Point barrel_pos = pos + barrel_vec;
			Vector dist_t = pos_t - barrel_pos;
			float laser_flight_time = (length(dist_t) / laser_speed) * 0.7;
			Vector aim_t = (pos_t + vel_t*laser_flight_time) - barrel_pos; // aim vector
		
			tc.yaw = 57.296 * atan2(aim_t.x, aim_t.z) - 180;
			tc.pitch = 57.296 * atan2(aim_t.y, sqrt(aim_t.z*aim_t.z + aim_t.x*aim_t.x));			
		
			if((fabs(current.yaw-tc.yaw)+fabs(current.pitch-tc.pitch)) < AIM_PRECISION){

				// make sure we don't hit our own buildings and turrets
				// FIXME, this doesn't seem to work, turrets hit other turrets -peter
				world::Line aim_line(pos+norm(aim_t)*RADIUS, pos_t);
				world::si_iterator si_i = wr->find_si(aim_line);
				world::st_iterator st_i = wr->find_st(aim_line);
				if(si_i==wr->end_si() && st_i==wr->end_st())
					tc.fire = true;
			}
			else
				tc.fire = false;
		}
		else{
			tc.fire = false;
			fsm->state_transition(EVENT_TARGET_LOST);
		}
	}

/*
	void Turret::receive(Message m)
	{
		messages.push(m);
	}
*/

	void DummyTurret::think()
	{
		tc.yaw += 2;
		tc.pitch = 45+30*sin(tc.yaw/40);
		tc.fire=true;
	}

}
}
}
