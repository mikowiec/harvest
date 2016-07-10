#include "os/compat.h"

#include <iostream>
#include <string>
#include "os/debug.h"
#include "res/res.h"
#include "res/config.h"
#include "misc/parse.h"
#include "main/types_io.h"
#include "ai/navigation_graph.h"
// #include "world/world.h"

using namespace std;
using namespace reaper;
using namespace reaper::ai;


namespace reaper{
namespace ai{
namespace navigation_graph{

namespace { debug::DebugOutput dout("ai::navigation_graph"); }


Graph::Graph(void)
// : wr(world::World::get_ref()) 
{
	id_table.resize(1000);

}

Graph::~Graph(void)
{
	clear_graph();
}

NodePtr Graph::load_node(std::istream& is)
{
	res::ConfigEnv env(is, true);

	if(env.defined("node") && env.defined("point") && env.defined("link1")){
		NodePtr node = new Node;

		node->id = read<unsigned int>(env["node"]);	
		node->pos = read<Point>(env["point"]);
		
		int i = 1;
		while(env.defined("link" + misc::ltos(i))){
			node->links.push_back(read<Link>(env["link" + misc::ltos(i)]));
			++i;
		}
		dout << "node loaded\n";
		return node;
	} else {
		return 0;
	}
}

void Graph::build_graph(std::string filename)
{
	try {
		res::res_stream is(res::AI, filename, res::throw_on_error);

		res::ConfigEnv env(is, true);
		unsigned int nr_of_nodes = read<unsigned int>(env["nr_of_nodes"]);
		id_table.resize(nr_of_nodes);

		do {
			NodePtr n = load_node(is);
			if(n == 0){ 
				break;
			} else {
				graph.push_back(n);
				id_table[n->id] = n;
			}
		} while(true);

	} catch(res::resource_not_found e) {
		dout << e.what() << '\n';
		dout << "Building graph from terrain data...";
		dout.flush();


		// TODO : Implement graph builder


		dout << "done!\n";
	}
}

void Graph::clear_graph(void)
{
	for(list<NodePtr>::iterator it = graph.begin(); it != graph.end(); it++){
		NodePtr node = (*it);
		delete node;
		dout << "node deleted\n";
	}
	graph.clear();
	id_table.clear();
}

void Graph::find_onoff_ramps(Point& start, Point& dest, NodePtr &on, NodePtr &off)
{
	if(!graph.empty()){
		
		// TODO: implement algorithm
		
		on = graph.front();
		off = graph.back();
	} else {
		on = off = 0;
	}
}


}
}
}

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:14 $
 * $Log: navigation_graph.cpp,v $
 * Revision 1.12  2002/12/29 17:04:14  pstrand
 * hw/ moved to os/
 *
 * Revision 1.11  2002/04/11 01:02:04  pstrand
 * explicit res_stream exception...
 *
 * Revision 1.10  2002/02/01 15:40:30  niklas
 * no message
 *
 * Revision 1.9  2002/01/27 18:26:06  niklas
 * *** empty log message ***
 *
 * Revision 1.8  2002/01/27 16:40:39  niklas
 * *** empty log message ***
 *
 * Revision 1.7  2002/01/26 23:27:11  macke
 * Matrox fix, plus some misc stuff..
 *
 * Revision 1.6  2002/01/24 21:33:16  peter
 * använd hellre misc::ltos&stol istf atoi&itoa...
 *
 * Revision 1.5  2002/01/24 20:24:45  niklas
 * *** empty log message ***
 *
 * Revision 1.4  2002/01/24 17:49:03  niklas
 * Inläsning av grafdata från fil
 *
 * Revision 1.3  2002/01/23 17:24:32  niklas
 * *** empty log message ***
 *
 * Revision 1.2  2002/01/22 23:55:04  niklas
 * no message
 *
 * Revision 1.1  2002/01/22 23:53:48  niklas
 * no message
 *
*/

