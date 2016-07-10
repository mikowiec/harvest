/*
 * $Author: pstrand $
 * $Date: 2003/06/13 14:19:38 $
 * $Log: gvbase.cpp,v $
 * Revision 1.14  2003/06/13 14:19:38  pstrand
 * move code from quadtree.h to quadtree.cpp, and gvs thinking
 *
 * Revision 1.13  2003/06/11 15:15:17  pstrand
 * småfixar..
 *
 * Revision 1.12  2003/06/04 16:21:24  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.11  2002/12/29 17:04:14  pstrand
 * hw/ moved to os/
 *
 * Revision 1.10  2001/12/03 18:06:13  peter
 * object/base.h-meck...
 *
 * Revision 1.9  2001/10/01 10:43:45  peter
 * fix...
 *
 * Revision 1.8  2001/08/06 12:16:01  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.7.2.1  2001/08/05 14:01:21  peter
 * objektmeck...
 *
 * Revision 1.7  2001/07/30 23:43:11  macke
 * Häpp, då var det kört.
 *
 * Revision 1.6  2001/07/09 13:33:04  peter
 * gcc-3.0 fixar
 *
 * Revision 1.5  2001/05/13 17:19:23  niklas
 * fix
 *
 * Revision 1.4  2001/05/10 10:11:17  niklas
 * småfix
 *
 * Revision 1.3  2001/05/09 21:03:00  niklas
 * småfix
 *
 * Revision 1.2  2001/05/09 10:04:18  niklas
 * småfix
 *
 * Revision 1.1  2001/05/06 14:18:57  niklas
 * gvs uppdelade på containers och turrets, gvturret är inte klar än...
 *
 * Revision 1.2  2001/05/06 00:15:50  niklas
 * småfix
 *
 * Revision 1.1  2001/05/05 18:41:25  niklas
 * la till ai för ground vehicle, bara styrningen än så länge...
 *
*/

#include "os/compat.h"
#include "object/ai.h"
#include "world/world.h"
#include "main/types_ops.h"
#include "object/base_data.h"
#include "object/controls.h"
#include "object/mkinfo.h"
#include <float.h>
#include <math.h>

namespace reaper{
namespace object{
namespace ai{

	GVBase::GVBase(MkInfo mk, CompanyID cid, object::GetDyn* d, controls::GroundShip &sctrl)
		: company(cid), dyn(d), sc(sctrl)
	{
		wp_it = waypoints.end(); // no waypoints yet
	}

	GVBase::~GVBase(void) {}

	void GVBase::navigate(const Vector vel, const Vector dir, controls::GroundShip& sc)
	{
		// TODO: obsticle avoidance

		Vector loc_dir;

		loc_dir.x = vel.x*dir.x + vel.z*dir.z;
		loc_dir.z = (-vel.z)*dir.x + vel.x*dir.z;

		float a = atan2(loc_dir.z, loc_dir.x);
		
		if(fabs(a)>PI/32)
			sc.turn = copysign(1.0,a*-1); // big angle, turn fast 
		else if(fabs(a)>PI/64)
			sc.turn = copysign(0.5,a*-1); // medium angle, turn medium
		else
			sc.turn = (a / PI)*-1;        // small angle, finetune		
	}

	void GVBase::add_waypoint(const Point& p)
	{	
		if(waypoints.empty()) {
			waypoints.push_back(Point(p.x, wr->get_altitude(Point2D(p.x,p.z)), p.z));
			wp_it = waypoints.begin();
		} else
			waypoints.push_back(Point(p.x, wr->get_altitude(Point2D(p.x,p.z)), p.z));
	}

	void GVBase::erase_waypoints(void)
	{
		waypoints.clear();
	}

/*
	void GVBase::receive(Message m)
	{
		messages.push(m);
	}
*/

}
}
}
