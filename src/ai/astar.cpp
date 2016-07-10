#include "os/compat.h"

#include <algorithm>
#include <cmath>
#include "os/debug.h"
#include "ai/astar.h"
#include "ai/exceptions.h"
#include "main/types_ops.h"

namespace reaper{
namespace ai{
namespace astar{

namespace { debug::DebugOutput dout("ai::astar"); }

Node::Node(void)
{}

Node::Node(navigation_graph::NodePtr &ndata) :
	navidata(ndata)
{}

AStar::AStar(navigation_graph::Graph &ng) :
	navigraph(ng),
	current(NULL),
	search_state(SEARCH_STATE_NOT_INITIALISED)
{
	start = new Node;
	goal = new Node;

}

AStar::~AStar(void) 
{
	open.clear();
	closed.clear();
	successors.clear();
	delete start;
	delete goal;
}
	
// Initiallize a new search
void AStar::new_search(Point &sp, Point &gp)
{
	navigraph.find_onoff_ramps(sp, gp, start->navidata, goal->navidata);

	// Initialise the A* specific parts of the start node
	start->g = 0.0; 
	start->h = estimate_cost_to_goal(start);
	start->f = start->g + start->h;
	start->parent = 0;

	// Push the start node on the open list (heap)
	open.push_back(start);
	std::push_heap(open.begin(), open.end(), NodeCmp());

	steps = 0;
	search_state = SEARCH_STATE_SEARCHING;

}



// Take one step forward in the search and return the resulting state
int AStar::search_step(void)
{
	// Break if the search is not initialised or the search is finished
	if(search_state != SEARCH_STATE_SEARCHING)
		return search_state;

	// If the open list is empty, there is no solution,
	// set state to failed and return
	if(open.empty())
		return search_state = SEARCH_STATE_FAILED;
	
	steps++;

	// Get the best node from the open list (pop the heap)
	Node* n = open.front();
	std::pop_heap(open.begin(), open.end(), NodeCmp());
	open.pop_back();

	// Check if this was the goal, if it was we're done
	if(n == goal)
	{
		// If the goal and start is not the same node,
		// reconstruct the solution path
		if(n != start)
		{
			// Set the child pointers in each node 
			// (except goal which has no child)
			Node* child_node = goal;
			Node* parent_node = goal->parent;

			do {
				// set pointer
				parent_node->child = child_node;
				// move on to next
				child_node  = parent_node;
				parent_node = parent_node->parent;
			} while(child_node != start); // start is always the first node
		}

		return search_state = SEARCH_STATE_SUCCEEDED;

	} else { // if not the goal

		// We now need to get the successors (neigbours) of this node,
		// the neighbours are placed in the list successors
		get_successors(n);
		
		// Handle each successor
		for(NodeListIterator succ = successors.begin(); succ != successors.end(); succ++)
		{
			// Calculate the cost from start to this node
			float newg = n->g + get_cost(n,(*succ));

			// Now we need to find whether the node is already on the open or 
			// closed lists. If it is but the node that is already on them is
			// better (lower g) then we can forget about this successor

			// First linear search of open list to find node
			// TODO: Improve to better complexity than linear

			NodeListIterator open_it;
			for(open_it = open.begin(); open_it != open.end(); open_it++){
				if((*open_it) == (*succ))
					break;
			}

			if((open_it != open.end()) && ((*open_it)->g <= newg))
				continue;


			// Now do the same check on closed list

			NodeListIterator closed_it;
			for(closed_it = closed.begin(); closed_it != closed.end(); closed_it++){
				if((*closed_it) == (*succ))
					break;
			}

			if((closed_it != closed.end()) && ((*closed_it)->g <= newg))
				continue;

			// This node is the best node so far so
			// lets keep it and set up its A* data

			(*succ)->parent = n;
			(*succ)->g = newg;
			(*succ)->h = estimate_cost_to_goal((*succ));
			(*succ)->f = (*succ)->g + (*succ)->h;
			(*succ)->visited = true;

			// Remove successor from closed if it was on it
			if(closed_it != closed.end())
				closed.erase(closed_it);

			// If not in open yet, push node onto open
			// else make sure to keep the heap structure
			if(open_it == open.end()){
				open.push_back((*succ));
				std::push_heap(open.begin(), open.end(), NodeCmp());
			} else {
				std::make_heap(open.begin(), open.end(), NodeCmp());
			}
		}

		// Push n onto closed, as we have expanded it now
		closed.push_back(n);
	}

 	return search_state; // return the resulting state of this search step
}

/*
// Methods for traversing a solution when a search has ended 
Node* AStar::get_solution_start(void)
{
	current = start;
	return current;
}

Node* AStar::get_solution_end(void)
{
	current = goal;
	return current;
}

Node* AStar::get_solution_next(void)
{
	if(current){
		if(current->child){
			current = current->child;
			return current;
		}			
	}
	return NULL;
}

Node* AStar::get_solution_prev(void)
{
	if(current){
		if(current->parent){
			current = current->parent;
			return current;
		}				
	}
	return NULL;
}

// Methods to view the open list during a search
Node* AStar::get_open_start(void)
{
	float f,g,h;
	return get_open_start(f,g,h);
}

Node* AStar::get_open_start(float& f, float& g, float& h)
{
	dbg_open_it = open.begin();
	if(dbg_open_it != open.end()){
		f = (*dbg_open_it)->f;
		g = (*dbg_open_it)->g;
		h = (*dbg_open_it)->h;
		return (*dbg_open_it);
	}
	return NULL;
}

Node* AStar::get_open_next(void)
{
	float f,g,h;
	return get_open_next(f,g,h);
}

Node* AStar::get_open_next(float& f, float& g, float& h)
{
	if(++dbg_open_it != open.end()){
		f = (*dbg_open_it)->f;
		g = (*dbg_open_it)->g;
		h = (*dbg_open_it)->h;
		return (*dbg_open_it);
	}
	return NULL;
}

// Methods to view the closed list during a search
Node* AStar::get_closed_start(void)
{
	float f,g,h;
	return get_closed_start(f,g,h);
}

Node* AStar::get_closed_start(float& f, float& g, float& h)
{
	dbg_closed_it = closed.begin();
	if(dbg_closed_it != closed.end()){
		f = (*dbg_closed_it)->f;
		g = (*dbg_closed_it)->g;
		h = (*dbg_closed_it)->h;
		return (*dbg_closed_it);
	}
	return NULL;
}

Node* AStar::get_closed_next(void)
{
	float f,g,h;
	return get_closed_next(f,g,h);
}

Node* AStar::get_closed_next(float& f, float& g, float& h)
{
	if(++dbg_closed_it != closed.end()){
		f = (*dbg_closed_it)->f;
		g = (*dbg_closed_it)->g;
		h = (*dbg_closed_it)->h;
		return (*dbg_closed_it);
	}
	return NULL;
}

// Return the number of steps taken in the search
int AStar::get_step_count(void) const
{
	return steps;
}

// Function for reseting the search and clearing all lists
void AStar::reset(void)
{
	open.clear();
	closed.clear();
	successors.clear();
	search_state = SEARCH_STATE_NOT_INITIALISED;
}

*/

void AStar::get_successors(Node* node)
{
	successors.clear();
	
	int parent_id = -1;
	if(node->parent) 
	{
		parent_id = node->parent->navidata->id;
	}
	
	// push each possible move except allowing the search to go backwards

	navigation_graph::LinkIterator it = node->navidata->links.begin();
	for(; it != node->navidata->links.end(); it++){
		if((*it).node_id != parent_id){
/*
			Node* succ = new Node;((*it));
			successors.push_back(succ);
*/		}
	}
}


float AStar::estimate_cost_to_goal(astar::Node* node)
{
	return length(goal->navidata->pos - node->navidata->pos);
}


float AStar::get_cost(astar::Node* node1, astar::Node* node2)
{
	navigation_graph::LinkIterator it = node1->navidata->links.begin();
	for(; it != node1->navidata->links.end(); it++){
		if((*it).node_id == node2->navidata->id)
			return (*it).cost;
	}	

	throw ai_error("Navigation graph error");;
}


}
}
}

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:14 $
 * $Log: astar.cpp,v $
 * Revision 1.8  2002/12/29 17:04:14  pstrand
 * hw/ moved to os/
 *
 * Revision 1.7  2002/02/18 11:50:10  peter
 * *** empty log message ***
 *
 * Revision 1.6  2002/02/01 15:40:30  niklas
 * no message
 *
 * Revision 1.5  2002/01/28 00:50:59  macke
 * hw/compat.h !!
 *
 * Revision 1.4  2002/01/27 18:26:06  niklas
 * *** empty log message ***
 *
 * Revision 1.3  2002/01/27 16:40:38  niklas
 * *** empty log message ***
 *
 * Revision 1.2  2002/01/24 20:24:44  niklas
 * *** empty log message ***
 *
 * Revision 1.1  2002/01/23 17:24:32  niklas
 * *** empty log message ***
 *
*/
