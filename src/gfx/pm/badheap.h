#ifndef REAPER_GFX_PM_BADHEAP_H
#define REAPER_GFX_PM_BADHEAP_H

#include "pm_types.h"

namespace reaper 
{
namespace gfx
{
namespace pm
{

class Pair_node;

typedef Pair_node* Pair_node_ptr;

typedef Pair_node_ptr Slowheap_index;

typedef std::list<Pair_node_ptr> Slowheap_index_list;

class Pair_node{
public:
	Pairdat pd;
	Pair_node_ptr prev;
	Pair_node_ptr next;

	Pair_node(Pairdat data);
};

class Slowheap{
private:
	Pair_node_ptr first, last;
public:

	Slowheap(void):first(NULL), last(NULL) {}

	Pair_node_ptr insert(Pairdat pd);
	bool remove(Pair_node_ptr);
	bool update(Pair_node_ptr);
	Pair_node_ptr pop(void);

};

}
}
}

#endif

