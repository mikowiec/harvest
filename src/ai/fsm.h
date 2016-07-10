#ifndef REAPER_AI_FSM_H
#define REAPER_AI_FSM_H

#include <vector>
#include <map>

using namespace std;

namespace reaper{
namespace ai{

///Generic finite state machine
namespace fsm{

        class State{
                /// Unique id of this state
                int id;
                /// Maximum number of transitions supported
                unsigned max_transitions;
                /// Vector of inputs for transitions
                vector<int> inputs;
                /// Vector of output states
                vector<int> outputs;
                
        public:
                /// Constructor, accepts the unique id of this state and the number
                /// of transitions to support
                State(int state_id, unsigned transitions);
                /// Cleans up allocated arrays
                ~State();

                /// Get id of this state
                int get_id();

                /// Add a state transition
                void add_transition(int input, int output_state);
                /// Delete a state transition
                void delete_transition(int output_state);

                /// Get output state corresponding to given input
                int get_output(int input);
        };

        typedef map<int, State*, less<int> > StateMap;
        
        class FSM{
                int current_state;
                StateMap states;
                
        public:
                /// Constructor, sets the initial state of the FSM
                FSM(int state_id);
                /// Cleans up memory
                ~FSM();

                /// Returns id of the current state
                int get_current_state();
                /// Set the current state
                void set_current_state(int state_id);

                /// Return the pointer of the state with matching state_id
                State* get_state(int state_id);
                /// Add a new state
                void add_state(State* new_state);
                /// Delete the state with the matching state_id
                void delete_state(int state_id);

                /// Perform a state transition based on input and current state,
                /// returns the current state after the transition
                int state_transition(int input);
        };


}        
}
}

#endif

/*
 * $Author: peter $
 * $Date: 2001/08/06 12:16:01 $
 * $Log: fsm.h,v $
 * Revision 1.4  2001/08/06 12:16:01  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.3.4.1  2001/08/03 13:43:44  peter
 * pragma once obsolete...
 *
 * Revision 1.3  2001/07/06 01:47:05  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.2  2001/05/06 00:13:12  niklas
 * städ
 *
 * Revision 1.1  2001/01/13 20:51:46  niklas
 * Ändra namn och lite annat i fsm
 *
 */

