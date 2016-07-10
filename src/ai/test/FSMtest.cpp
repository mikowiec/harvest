#include <iostream.h>
#include "ai\fsm.h"

using namespace reaper::ai::fsm;

enum turret_states {IDLE=1, AIMING, FIREING};
enum events {ENEMY_DETECTED, ENEMY_IN_AIM, AIM_LOST, ENEMY_KILLED};

void print_state(int state_id)
{
	cout << state_id << ".";
	switch(state_id)
	{
		case IDLE:
			cout << "IDLE\n";
			break;
		case AIMING:
			cout << "AIMING\n";
			break;
		case FIREING:
			cout << "FIREING\n";
			break;	
	}

}


void main()
{
	State *turret_state1 = NULL;
	turret_state1 = new State(IDLE, 1);	
	turret_state1->add_transition(ENEMY_DETECTED, AIMING);
	
	State *turret_state2 = NULL;
	turret_state2 = new State(AIMING, 2);
	turret_state2->add_transition(ENEMY_IN_AIM, FIREING);
	turret_state2->add_transition(ENEMY_KILLED, IDLE);

	State *turret_state3 = NULL;
	turret_state3 = new State(FIREING, 2);
	turret_state3->add_transition(ENEMY_KILLED, IDLE);
	turret_state3->add_transition(AIM_LOST, AIMING);

	FSM *turret_fsm = NULL;
	turret_fsm = new FSM(IDLE);
	turret_fsm->add_state(turret_state1);
	turret_fsm->add_state(turret_state2);
	turret_fsm->add_state(turret_state3);

	// test sequence
	print_state( turret_fsm->get_current_state() );
	turret_fsm->state_transition(ENEMY_DETECTED);
	print_state( turret_fsm->get_current_state() );
	turret_fsm->state_transition(ENEMY_IN_AIM);
	print_state( turret_fsm->get_current_state() );
	turret_fsm->state_transition(AIM_LOST);
	print_state( turret_fsm->get_current_state() );
	turret_fsm->state_transition(ENEMY_IN_AIM);
	print_state( turret_fsm->get_current_state() );
	turret_fsm->state_transition(ENEMY_KILLED);
	print_state( turret_fsm->get_current_state() );
}
