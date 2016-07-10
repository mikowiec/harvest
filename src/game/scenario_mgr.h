#ifndef REAPER_GAME_SCENARIO_MGR_H
#define REAPER_GAME_SCENARIO_MGR_H

#include "ai/fsm.h"
#include "ai/msg.h"
#include "game/scenario_states.h"
#include "misc/uniqueptr.h"
#include "game/mission.h"

#include <string>
#include <set>

using namespace reaper::ai;

namespace reaper{
namespace game{
/// Game scenario handling
namespace scenario{
	
class ScenarioMgr;
typedef reaper::misc::UniquePtr<ScenarioMgr> ScenarioRef;

class ScenarioMgr
{
	friend class misc::UniquePtr<ScenarioMgr>;

	float time;
	ObjectMgr om;
	MsgQueue messages;

//	fsm::FSM* ss;
	misc::SmartPtr<Scenario> scenario;
	std::map<std::string, StateBase*> states;
	std::string current;

	ScenarioMgr();

	std::string info;
	bool draw_flag;
	float drawtime;
	int count;

	void draw_file(const std::string&);
public:
	~ScenarioMgr();

	void init(const std::string& scenario);

	void destroy();
			
	void update(double timediff);
	void receive(Message msg);
	
	void set_info(std::string& info);
	void draw_info();

	void shutdown();

	void get_objectnames(std::set<std::string>&);
};

}
}
}
#endif

/*
 * $Author: pstrand $
 * $Date: 2002/09/26 06:28:53 $
 * $Log: scenario_mgr.h,v $
 * Revision 1.29  2002/09/26 06:28:53  pstrand
 * game/state.{h|cpp} is no more
 *
 * Revision 1.28  2002/05/04 11:12:37  pstrand
 * *** empty log message ***
 *
 * Revision 1.27  2002/04/18 01:18:28  pstrand
 * scenario in data files now
 *
 * Revision 1.26  2002/04/11 01:17:45  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.25  2002/03/27 10:32:51  pstrand
 * *** empty log message ***
 *
 * Revision 1.24  2002/03/26 02:00:16  pstrand
 * preload
 *
 * Revision 1.23  2002/03/24 17:34:38  pstrand
 * option to use only game_start_*
 *
 * Revision 1.22  2002/03/19 20:41:45  pstrand
 * reorg..
 *
 * Revision 1.21  2002/01/17 04:57:27  peter
 * *** empty log message ***
 *
 * Revision 1.20  2002/01/03 17:36:50  peter
 * ny ansats på spara/ladda!
 *
 * Revision 1.19  2001/12/15 22:59:03  peter
 * *** empty log message ***
 *
 * Revision 1.18  2001/11/10 13:57:01  peter
 * minnesfixar...
 *
 * Revision 1.17  2001/11/10 11:58:29  peter
 * diverse minnesfixar och strul...
 *
 * Revision 1.16  2001/08/06 12:16:09  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.15.2.1  2001/08/03 13:43:50  peter
 * pragma once obsolete...
 *
 * Revision 1.15  2001/07/27 15:43:53  peter
 * get local player from world...
 *
 * Revision 1.14  2001/07/24 23:52:45  macke
 * Jo, explicit ska det vara (fel på annat ställe)..  rättade till lite array-fel också..
 *
 * Revision 1.13  2001/07/09 13:33:05  peter
 * gcc-3.0 fixar
 *
 * Revision 1.12  2001/07/06 01:47:08  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.11  2001/05/14 21:12:13  peter
 * *** empty log message ***
 *
 * Revision 1.10  2001/05/14 15:15:33  niklas
 * grafisk textutskrift
 *
 *
 */

