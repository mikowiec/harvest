
#ifndef REAPER_PHYS_REGISTER_H
#define REAPER_PHYS_REGISTER_H


#include <phys/accessor.h>


namespace reaper {
namespace phys {


class Reg {

public:
	int insert(PrimAccessor* prim);
	int insert(SimAccessor* sim);
	int insert(DynAccessor* dyn);
	int insert(ShotAccessor* shot);

};


}
}

#endif

