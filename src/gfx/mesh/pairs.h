#ifndef REAPER_MESH_PAIRS_H
#define REAPER_MESH_PAIRS_H

#include "shared.h"
#include <set>


namespace reaper 
{
namespace gfx
{
namespace mesh
{

/*************************************************************
/                 CLASS: Pairs
/
/	Pairs is a set of vertex-pairs, which are made in order
/	to create the initial contractions.
/*************************************************************/

class Pairs {
private:
	std::set<Pair> pairset;

public:
	Pairs(){};

	void insert(int a, int b){pairset.insert(Pair(a,b));}
	void insert(Pair &p){pairset.insert(p);}
	Pair pop(){
		return mesh::pop(pairset);
	}
	bool is_empty();

	int size(){return pairset.size();}

	void report(bool verbose);
};

}
}
}

#endif
