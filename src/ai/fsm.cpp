/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:14 $
 * $Log: fsm.cpp,v $
 * Revision 1.6  2002/12/29 17:04:14  pstrand
 * hw/ moved to os/
 *
 * Revision 1.5  2002/01/11 21:27:57  peter
 * reserve -> resize!
 *
 * Revision 1.4  2001/11/26 10:46:01  peter
 * kompilatorgnäll... ;)
 *
 * Revision 1.3  2001/01/20 02:29:16  peter
 * antar att det ska vara såhär... ;)
 *
 * Revision 1.2  2001/01/13 22:44:32  niklas
 * *** empty log message ***
 *
 * Revision 1.1  2001/01/13 20:51:45  niklas
 * Ändra namn och lite annat i fsm
 *
 * Revision 1.2  2001/01/04 23:07:07  niklas
 * gjorde klart FSM-klassen
 *
 * Revision 1.1  2000/12/04 19:02:41  niklas
 * *** empty log message ***
 *
 */

//#include <iostream.h>
#include "os/compat.h"
#include "ai/fsm.h"

namespace reaper{
namespace ai{
namespace fsm{

	State::State(int state_id, unsigned transitions)
	{
		id = state_id;
                
		if(transitions == 0)
                        max_transitions = 1;
                else
                        max_transitions = transitions;
                
                // Allocate memory and set all inputs and outputs to zero
                inputs.resize(max_transitions);
                for(unsigned i=0; i < max_transitions; i++)
			inputs[i] = 0;

                outputs.resize(max_transitions);
                for(unsigned i=0; i < max_transitions; i++)
			outputs[i] = 0;
	}
        
        State::~State()
        {
		// cout << "State destructor called!";
        }
        
        int State::get_id() { return id; }
        
        void State::add_transition(int input, int output_state)
        {
		unsigned i;

                /* Find the first non-zero offset in outputs array and use it to store
                   the input and output_state in inputs and outputs arrays */
                for(i=0; i < max_transitions; i++){
                        if(outputs[i]==0)
                                break;
                }
                if(i < max_transitions){
                        inputs[i] = input;
                        outputs[i] = output_state;
                }
        }
        
        void State::delete_transition(int output_state)
        {
		unsigned i;
                // Find the offset of the state to be removed 
                for(i=0; i<max_transitions; i++){
                        if(outputs[i] == output_state)
                                break;
                }
                /* If found, remove this output and its input transition value
                   and shift down the remaning contents in inputs and outputs */
                if(i < max_transitions){
                        inputs[i] = 0;
                        outputs[i] = 0;
			
                        for(; i	< (max_transitions-1); i++){
				if(outputs[i] == 0)
					break;
				inputs[i] = inputs[i+1];
				outputs[i] = outputs[i+1];
			}
		}       
        }
        
        int State::get_output(int input)
        {
		int output_state = id; // state to be returned
		
		// look up output matching input
		for(unsigned i=0; i<max_transitions; i++){
			if(outputs[i]==0) // zero indicates the end of the array
				break;
			if(input == inputs[i]){
				output_state = outputs[i];
				break;
			}
		}
		
		/* Return the transitioned output state, or if no output was matched by 
		   the input return the current state (id) */	
		return output_state;
        }

	FSM::FSM(int state_id)
	{
		current_state = state_id;
	}
                
	FSM::~FSM()
	{
		State *state = NULL;
		StateMap::iterator it;

		// if objects exists in the map states
		if(!states.empty()){
			// delete all of them
			for(it = states.begin(); it != states.end(); it++){
				state = (State*)((*it).second);
				if(state != NULL)
					delete state;
			}
		}
		// cout << "FSM destructor called!";
	}

        int FSM::get_current_state()
	{
		return current_state;
	}
           
	void FSM::set_current_state(int state_id)
	{
		current_state = state_id;
	}
		
                
        State *FSM::get_state(int state_id)
	{
		State *state = NULL;
		StateMap::iterator it;

		// if states exists
		if(!states.empty()){
			// try to find this state object
			it = states.find(state_id);
			if(it != states.end())
				state = (State*)((*it).second);
		}
		return state;
	}			

                
        void FSM::add_state(State* new_state)
	{
		StateMap::iterator it;

		// see if this state is already in the map
		if(!states.empty()){
			it = states.find(new_state->get_id());
			if(it != states.end())
				// if it is then exit
				if((*it).second != NULL)
					return;
			
		}
		// else put it into the map
		states.insert( StateMap::value_type(new_state->get_id(), new_state) );
	}
        
	void FSM::delete_state(int state_id)
	{
		State *state = NULL;
		StateMap::iterator it;

		// try to find this state in the map
		if(!states.empty()){
			it = states.find(state_id);
			if(it != states.end()){
				state = (*it).second;
				// confirm this was the right one
				if(state != NULL && state->get_id() == state_id){
					states.erase(it); // remove it from map
					delete state;	  // delete the object itself
				}
			}
		}
	}

	int FSM::state_transition(int input)
	{
		// current_state must be set
		if(!current_state)
			return current_state; // no transition made
		
		// get the object pointer of the current state
		State *state = get_state(current_state);
		
		if(state == NULL){ // problem!!!
			current_state = 0;
			return current_state;
		}
		
		// pass along the input transition value and let the State
		// do the transition and save the output state returned
		current_state = state->get_output(input);

		return current_state; // transition made
	}
        
}
}
}
