#ifndef CLOSE_COUNTER_H
#define CLOSE_COUNTER_H

#include "os/compat.h"
#include "bucket.h"
#include "misc/hash.h"
#include "misc/smartptr.h"

#include <utility>
using std::pair;
using std::make_pair;
#include "phys/pairs.h"

typedef reaper::misc::SmartPtr<reaper::phys::Pair> pairPtr;

using reaper::misc::hash_map;

namespace reaper{
	namespace phys{

class hfunc
{
public:
	inline int operator()(const pair<objId,objId>& p) const { return p.first*5 + p.second*2;}
};


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class close_counter  
{
public:
	close_counter();
	~close_counter();
        int increase( objId,  objId);
        int decrease( objId,  objId);
	int get_counter(objId, objId) ;
	
private:
        hash_map< pair<objId,objId>,int, hfunc > counters;
};



}
}

#endif // !defined(CLOSE_COUNTER_H
