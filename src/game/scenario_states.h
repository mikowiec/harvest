/*
 * $Author: pstrand $
 * $Date: 2003/06/04 16:27:30 $
 * $Log: scenario_states.h,v $
 * Revision 1.15  2003/06/04 16:27:30  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.14  2002/09/29 12:35:52  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.13  2002/04/18 01:18:28  pstrand
 * scenario in data files now
 *
 * Revision 1.12  2002/03/27 10:32:51  pstrand
 * *** empty log message ***
 *
 * Revision 1.11  2002/02/18 11:49:22  peter
 * objektladdning
 *
 * Revision 1.10  2002/01/10 23:09:05  macke
 * massa bök
 *
 * Revision 1.9  2001/08/06 12:16:10  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.8.4.1  2001/08/03 13:43:50  peter
 * pragma once obsolete...
 *
 * Revision 1.8  2001/07/06 01:47:08  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.7  2001/05/13 17:26:03  niklas
 * info utskrift
 *
 * Revision 1.6  2001/05/06 14:20:17  niklas
 * funktion för meddeladeutskrift mellan delscenarion, ej klar än...
 *
 * Revision 1.5  2001/05/06 00:17:32  niklas
 * referens till meddelandekön
 */

#ifndef REAPER_GAME_SCENARIO_STATES_H
#define REAPER_GAME_SCENARIO_STATES_H

#include "ai/fsm.h"
#include "ai/msg.h"
#include "game/object_mgr.h"
#include "game/mission.h"

namespace reaper {
namespace game {
namespace scenario {

using reaper::ai::fsm::FSM;
using reaper::ai::MsgQueue;

//const int nr_of_states = 6;

/*
enum ScenarioState
{
	GAME_START = 1,
	DEST_KANYON,
	PROT_CONVOY,
	DEST_CONVOY,
	DEST_VILLAGE,
	GAME_END
};

enum ScenarioEvent
{
	NO_EVENT = 0,
	TIME_EXP,
	MISSION_SUCCESS,
	MISSION_FAILED
};
*/

class StateBase
{
protected:
	ObjectMgr& om;
	MsgQueue& mq;

public:
	StateBase(ObjectMgr& o, MsgQueue& m);
	virtual ~StateBase();

	virtual std::string on_enter() = 0;
	virtual std::string update(float time) = 0;
	virtual void on_exit() = 0;
};

class MissionState : public StateBase
{
	Mission* m;
	bool dead;
	void on_death();
public:
	MissionState(Mission* m, ObjectMgr& o, MsgQueue& q);

	std::string on_enter();
	std::string update(float time);
	void on_exit();

};

/*
class GameStart : public StateBase
{					
public:
	GameStart(FSM* s, ObjectMgr& o, MsgQueue& m, double& t);
	~GameStart(void){};

	void on_enter(void);
	void update(void);
	void on_exit(void);
};

class GameStartSmall : public StateBase
{					
public:
	GameStartSmall(FSM* s, ObjectMgr& o, MsgQueue& m, double& t);
	~GameStartSmall(void){};

	void on_enter(void);
	void update(void);
	void on_exit(void);
};


class DestKanyon : public StateBase
{
public:
	DestKanyon(FSM* s, ObjectMgr& o, MsgQueue& m, double& t);
	~DestKanyon(void);

	void on_enter(void);
	void update(void);
	void on_exit(void);
};

class ProtConvoy : public StateBase
{
public:
	ProtConvoy(FSM* s, ObjectMgr& o, MsgQueue& m, double& t);
	~ProtConvoy(void);

	void on_enter(void);
	void update(void);
	void on_exit(void);
};

class DestConvoy : public StateBase
{
public:
	DestConvoy(FSM* s, ObjectMgr& o, MsgQueue& m, double& t);
	~DestConvoy(void);

	void on_enter(void);
	void update(void);
	void on_exit(void);
};

class DestVillage : public StateBase
{
public:
	DestVillage(FSM* s, ObjectMgr& o, MsgQueue& m, double& t);
	~DestVillage(void);

	void on_enter(void);
	void update(void);
	void on_exit(void);
};

class GameEnd : public StateBase
{
	int count;
public:
	GameEnd(FSM* s, ObjectMgr& o, MsgQueue& m, double& t);
	~GameEnd(void){};

	void on_enter(void);
	void update(void);
	void on_exit(void);
};
*/

}
}
}
#endif

