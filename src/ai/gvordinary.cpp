/*
 * $Author: pstrand $
 * $Date: 2003/06/04 16:21:24 $
 * $Log: gvordinary.cpp,v $
 * Revision 1.11  2003/06/04 16:21:24  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.10  2002/12/29 17:04:14  pstrand
 * hw/ moved to os/
 *
 * Revision 1.9  2001/12/03 18:06:14  peter
 * object/base.h-meck...
 *
 * Revision 1.8  2001/08/06 12:16:01  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.7.2.1  2001/08/05 14:01:22  peter
 * objektmeck...
 *
 * Revision 1.7  2001/07/30 23:43:11  macke
 * Häpp, då var det kört.
 *
 * Revision 1.6  2001/05/14 00:26:43  niklas
 * högre hastighet
 *
 * Revision 1.5  2001/05/13 17:22:23  niklas
 * städ
 *
 * Revision 1.4  2001/05/12 10:00:14  niklas
 * småfix
 *
 * Revision 1.3  2001/05/10 10:14:03  niklas
 * småfix
 *
 * Revision 1.2  2001/05/10 02:02:43  niklas
 * småfix
 *
 * Revision 1.1  2001/05/09 21:03:45  niklas
 * ai till GroundOrdinary
 *
*/

#include "os/compat.h"
#include "object/ai.h"
#include "object/base_data.h"
#include "object/controls.h"
#include "object/mkinfo.h"
#include "object/accessor.h"
#include "main/types_ops.h"

namespace reaper{
namespace object{
namespace ai{

	enum GVOrdinarySpec
	{
		DIST_WAYPOINT_REACHED = 50
	};

	GVOrdinary::GVOrdinary(MkInfo mk, CompanyID cid, object::GetDyn* d, controls::GroundShip &sctrl)
		: GVBase(mk,cid,d,sctrl)
	{}

	GVOrdinary::~GVOrdinary()
	{	
	}

	void GVOrdinary::think()
	{		
		
		if(!waypoints.empty()){
			
			Point pos = dyn->pos();
			Point pwp = (*wp_it);   // Position of waypoint
			Vector dwp = pwp - pos; // Direction to waypoint
			
			// Check if waypoint reached
			if(length(dwp) < DIST_WAYPOINT_REACHED){
				wp_it++;
				if(wp_it == waypoints.end())
					wp_it = waypoints.begin();
				return;
			}

			// set thrust
			sc.thrust = 1.0;

			// set turn
			navigate(dyn->velocity(), dwp, sc);
		}
	}
}
}
}
