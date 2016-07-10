#include "close_counter.h"

#include <iostream>
using std::cout;
using std::endl;

namespace reaper{
namespace phys{

close_counter::close_counter()
{

}

close_counter::~close_counter()
{

}

int close_counter::increase( objId o1,  objId o2)
{
        if(o2 > o1)
                std::swap(o1,o2);

	if(o1 == o2)
		return 0;   //Do not allow counter with same ids

	const pair<objId,objId> par(o1,o2);

        if(counters.elem(par) ){                
		int &old = counters[par];
		++old;
		return old;
	}
        else{
                counters.insert(par,1);
		return 1;
        }
}


int close_counter::decrease( objId o1,  objId o2)
{
        if(o2 > o1)
                std::swap(o1,o2);

	const pair<objId,objId> par(o1,o2);

        if(counters.elem(par ) ){               
		int &oldval = counters[par];
		--oldval;
		if(oldval <= 0)
			counters.erase(par);
		
		return oldval;
        }
        //if no such key then do nothing
	return 0;
}

int close_counter::get_counter(objId o1, objId o2) 
{
	if(o2 > o1)
                std::swap(o1,o2);

	pair<objId,objId> par(o1,o2);

        if(counters.elem(par))			 
		return counters[par];

        //if no such key
	return 0;
}

}
}//Namspace
