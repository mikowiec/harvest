
#ifndef PHYS_PHYSENGINE_INST_H
#define PHYS_PHYSENGINE_INST_H

#include "misc/hash.h"
#include "misc/singleton.h"

#include "main/types.h"
#include "phys/accessor.h"
#include "phys/collisionQueue.h"

namespace reaper {
namespace phys {
namespace impl {

typedef int objId;

class Engine
{
public:
	~Engine();

	void update_world(double start_time, double delta_time);

	int insert(PrimAccessor* prim);
	int insert(SimAccessor* sim);
	int insert(DynAccessor* dyn);
	int insert(ShotAccessor* shot);
	int insert(ShotAccessor* shot,double sim_time);

	void remove(objId id);

	int size();
	void shutdown();
	void reinit();

	bool is_dead(objId id);
private:
	friend class phys::Engine;
	friend class misc::singleton::Accessor<Engine>;
	Engine();

	bool to_insert(double framestop,const Pair& p);
	void startup();
	void clean_dead_objects();

	double frame_stop;
	bool simulating;
	float tri_interval;
	float since_last_tri;

	float last_garbage;

	hash::hash_map< objId, double >  dead_objects;
	PriorityQueue prio_queue;
	
	std::deque< PrimAccessor* >   sillys;
	std::deque< SimAccessor* >  statics;
	std::deque< DynAccessor* > dynamics;
	std::deque< ShotAccessor* >    shots;
	
	hash::hash_map< int, PrimAccessorPtr >   sillys_map;
	hash::hash_map< int, SimAccessorPtr >  statics_map;
	hash::hash_map< int, DynAccessorPtr > dynamics_map;
	hash::hash_map< int, ShotAccessorPtr >    shots_map;
	
};


}
}
}

#endif
