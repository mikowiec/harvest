#ifndef REAPER_AI_NAVIGATION_GRAPH_H
#define REAPER_AI_NAVIGATION_GRAPH_H

#include <list>
#include <string>
#include "main/types.h"
#include "main/types_io.h"
#include "misc/parse.h"
//#include "world/worldref.h"

namespace reaper{
namespace ai{
namespace navigation_graph{


enum LinkType {
	LINK_TYPE_LAND,
	LINK_TYPE_AIR,
	LINK_TYPE_WATER
};


/// An edge in the navigation graph
class Link
{
public:
	LinkType type;
	float cost;
	unsigned int node_id;
};

/// A node in the navigation graph
class Node
{
public:
	unsigned int id;
	Point pos;
	std::list<Link> links;
};

typedef Node* NodePtr;
typedef std::list<Link>::iterator LinkIterator;


class Graph
{
	std::list<NodePtr> graph; // TODO: change from list to QuadTree
	std::vector<NodePtr> id_table;

//	world::WorldRef wr; // referens to the world module
public:
	Graph(void);
	~Graph(void);

	void build_graph(std::string filename);
	void clear_graph(void);

	void find_onoff_ramps(Point &start, Point &dest, NodePtr &on, NodePtr &off);

protected:
	NodePtr load_node(std::istream &io);
};



/// Output operator for Link
inline std::ostream& operator<<(std::ostream &os, const Link& l) 
{
        return os << '[' << l.type << ',' << l.cost << ',' << l.node_id << ']';
}

/// Input operator for Link
inline std::istream& operator>>(std::istream& is, Link& l)
{
	char junk;
	int tp;
	is >> tp >> junk >> l.cost >> junk >> l.node_id;
	l.type = static_cast<LinkType>(tp);
	return is;
}


}
}
}


#endif


/*
 * $Author: pstrand $
 * $Date: 2002/06/18 12:21:06 $
 * $Log: navigation_graph.h,v $
 * Revision 1.8  2002/06/18 12:21:06  pstrand
 * *** empty log message ***
 *
 * Revision 1.7  2002/02/01 15:40:31  niklas
 * no message
 *
 * Revision 1.6  2002/01/27 18:26:06  niklas
 * *** empty log message ***
 *
 * Revision 1.5  2002/01/27 16:40:39  niklas
 * *** empty log message ***
 *
 * Revision 1.4  2002/01/24 20:24:45  niklas
 * *** empty log message ***
 *
 * Revision 1.3  2002/01/24 17:49:03  niklas
 * Inläsning av grafdata från fil
 *
 * Revision 1.2  2002/01/23 17:24:33  niklas
 * *** empty log message ***
 *
 * Revision 1.1  2002/01/22 23:53:48  niklas
 * no message
 *
*/
