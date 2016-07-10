
#ifndef REAPER_PHYS_ENGINE_H
#define REAPER_PHYS_ENGINE_H

#include "misc/singleton.h"

#include "main/types.h"
#include "phys/accessor.h"

namespace reaper {
namespace phys {
namespace impl {
	class Engine;
}

typedef int objId;



class Engine : public misc::singleton::Base
{
	impl::Engine* i;
public:
	Engine();
	~Engine();

	void update_world(double start_time, double delta_time);

	void insert(PrimAccessor* prim);
	void insert(SimAccessor* sim);
	void insert(DynAccessor* dyn);
	void insert(ShotAccessor* shot);
	void insert(ShotAccessor* shot,double sim_time);

	void remove(objId id);

	int size();
	void shutdown();
//	void reinit();
};


typedef misc::singleton::Impl<Engine> Impl;
typedef misc::singleton::Accessor<Engine> PhysRef;

}
}

#endif
