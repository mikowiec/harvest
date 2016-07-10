/*
 * $Author: pstrand $
 * $Date: 2003/06/04 16:21:24 $
 * $Log: gvcontainer.cpp,v $
 * Revision 1.12  2003/06/04 16:21:24  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.11  2002/12/29 17:04:14  pstrand
 * hw/ moved to os/
 *
 * Revision 1.10  2002/09/29 12:35:52  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.9  2002/04/18 01:19:16  pstrand
 * scen_mgr::get_ref
 *
 * Revision 1.8  2001/12/03 18:06:13  peter
 * object/base.h-meck...
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
 * Revision 1.5  2001/05/13 17:19:43  niklas
 * fix
 *
 * Revision 1.4  2001/05/10 10:14:03  niklas
 * småfix
 *
 * Revision 1.3  2001/05/10 02:02:52  niklas
 * småfix
 *
 * Revision 1.2  2001/05/09 21:03:00  niklas
 * småfix
 *
 * Revision 1.1  2001/05/06 14:18:57  niklas
 * gvs uppdelade på containers och turrets, gvturret är inte klar än...
 *
*/

#include "os/compat.h"
#include "object/ai.h"
#include "game/scenario_mgr.h"
#include "object/base_data.h"
#include "object/controls.h"
#include "object/accessor.h"
#include "object/mkinfo.h"
#include "main/enums.h"
#include "main/types_ops.h"
 
namespace reaper{
namespace object{
namespace ai{

	enum GVContainerSpec
	{
		DIST_WAYPOINT_REACHED = 50
	};

	GVContainer::GVContainer(MkInfo mk, CompanyID cid, GetDyn* d, controls::GroundShip &sctrl)
		: GVBase(mk,cid,d,sctrl)
	{
		dest_reached = false;
	}

	GVContainer::~GVContainer()
	{	
	}

	void GVContainer::think()
	{
		if(dest_reached) return;

		Point pos = dyn->pos();

		if(!waypoints.empty()){
			
			Point pwp = (*wp_it);   // Position of waypoint
			Vector dwp = pwp - pos; // Direction to waypoint
			
			// Check if waypoint reached
			if(length(dwp) < DIST_WAYPOINT_REACHED){
				wp_it++;
				// if last one, destination is reached
				if(wp_it == waypoints.end()){
					// let the scenario manager know this
					game::scenario::ScenarioRef sr = game::scenario::ScenarioRef();
					sr->receive(Message(MSG_DEST_REACHED,company));
					sc.thrust = 0.0;
					dest_reached = true;
				}
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
