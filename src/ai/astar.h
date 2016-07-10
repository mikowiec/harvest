#ifndef REAPER_AI_A_STAR_H
#define REAPER_AI_A_STAR_H

#include <vector>
#include "ai/navigation_graph.h"


namespace reaper{
namespace ai{
namespace astar{

	enum SearchState {
		SEARCH_STATE_NOT_INITIALISED,
		SEARCH_STATE_SEARCHING,
		SEARCH_STATE_SUCCEEDED,
		SEARCH_STATE_FAILED
	};

	/// A node in the pathfinding graph, not to be confused with 
	/// a node in the navigation graph. Contains astar specific data.
	class Node
	{
	public:
		navigation_graph::NodePtr navidata;
		bool visited;

		Node *parent, *child;
					
		float g; // Cost of this node + it's predecessors
		float h; // Heuristic estimate of cost to goal
		float f; // Sum of g and h
		
		// Constructors
		Node(void);
		Node(navigation_graph::NodePtr &navidata);

		// For debugging purpouses
		void draw(void);
	};

	class NodeCmp
	{
	public:
		bool operator()(const Node *a, const Node *b) const
		{
			return (a->f > b->f);
		}
	};

	typedef std::vector<Node*>::iterator NodeListIterator;


	/// The astar search algorithm
	class AStar
	{
		navigation_graph::Graph &navigraph; // Reference to the navigation graph

		std::vector<Node*> open;   // The Open list (vector but used as a heap)
		std::vector<Node*> closed; // The Closed list
		
		std::vector<Node*> successors; // Used to hold the successors to the current node 
		
		Node* start; // Pointers to start
		Node* goal;  // and goal nodes

		Node* current; // Pointer to current node when traversing a solution
		
		int search_state; // Current search state
		int steps;	  // Number of search steps taken
		
		// Iterators for debugging the open and closed lists
		NodeListIterator dbg_open_it;
		NodeListIterator dbg_closed_it;

		void get_successors(Node* node);
		float estimate_cost_to_goal(Node* node);
		float get_cost(Node* node1, Node* node2);

	public:

		AStar(navigation_graph::Graph &ng);
		~AStar(void);

		// Initiallize the search
		void new_search(Point &sp, Point &gp);

		// Take one step forward in the search
		// and return the resulting state
		int search_step(void);

		// Methods for traversing a solution when a search has ended 
		Node* get_solution_start(void);
		Node* get_solution_end(void);
		Node* get_solution_next(void);
		Node* get_solution_prev(void);

		// Methods to view the open list during a search
		Node* get_open_start(void);
		Node* get_open_start(float& f, float& g, float& h);
		Node* get_open_next(void);
		Node* get_open_next(float& f, float& g, float& h);
		
		// Methods to view the closed list during a search
		Node* get_closed_start(void);
		Node* get_closed_start(float& f, float& g, float& h);
		Node* get_closed_next(void);
		Node* get_closed_next(float& f, float& g, float& h);

		// Return the number of steps taken in the search
		int get_step_count(void) const;

		// Function for reseting the search and clearing all lists
		void reset(void);
	};

}
}
}

#endif

/*
 * $Author: niklas $
 * $Date: 2002/02/01 15:40:30 $
 * $Log: astar.h,v $
 * Revision 1.4  2002/02/01 15:40:30  niklas
 * no message
 *
 * Revision 1.3  2002/01/27 16:40:39  niklas
 * *** empty log message ***
 *
 * Revision 1.2  2002/01/24 20:24:45  niklas
 * *** empty log message ***
 *
 * Revision 1.1  2002/01/23 17:24:32  niklas
 * *** empty log message ***
 *
*/
