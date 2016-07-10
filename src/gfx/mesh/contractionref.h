#ifndef REAPER_MESH_CONTRACTIONREF_H
#define REAPER_MESH_CONTRACTIONREF_H



#include "contraction.h"

#include <list>

#include "main/types.h"
#include "os/compat.h"


namespace reaper 
{
namespace gfx
{
namespace mesh
{

/*************************************************************
/                   CLASS: ContractionRef
/
/ Holds reference to a Contraction. Use for distribution and
/ dynamic containers (sorted).
/*************************************************************/
class ContractionRef {
public:
	ContractionRef(std::list<Contraction>::iterator i):ref(i){}

	std::list<Contraction>::iterator ref;

	bool operator<(ContractionRef &c1){
		return ( (*ref).get_cost() < (*c1.ref).get_cost());
	}

	bool operator==(ContractionRef &c1){
		return ( (*ref).get_cost() == (*c1.ref).get_cost());
	}

};

}
}
}

#endif
