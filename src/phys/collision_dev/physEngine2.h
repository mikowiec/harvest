#ifndef PHYS_PHYSENGINE2_H
#define PHYS_PHYSENGINE2_H

#include "os/compat.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "misc/hash.h"

#include "misc/uniqueptr.h"
#include "object/baseptr.h"
#include "world/worldref.h"

#include "bucket.h"
#include "close_counter.h"
#include "coll_hash.h"
#include "collisionQueue.h"

#include <vector>
using std::vector;

#include "misc/uniqueptr.h"

typedef int objId;

using namespace reaper::object;

namespace reaper{
namespace phys_dev{

class physEngine;
typedef reaper::misc::UniquePtr<reaper::phys_dev::physEngine> PhysRef;

class physEngine  
{
public:
	~physEngine();

	void update_world(double start_time, double delta_time);

	void insert(ShotPtr shot);
	void insert(ShotPtr shot,double sim_time);
	void insert(PlayerPtr ply);
	void insert(DynamicPtr dyn);
	void insert(SillyPtr sil);
	void insert(StaticPtr stat);

	void remove(objId id);

	static PhysRef create();
	static PhysRef get_ref();

	int size() const;
	void shutdown();
	void reinit();

private:
	
	physEngine();

	bool to_insert(double framestop,const Pair& p);
	void startup();

	reaper::world::WorldRef wr;
	
	double frame_stop;
	bool simulating;
	float tri_interval;
	float since_last_tri;

	hash_map< objId, double >  dead_object;
	PriorityQueue prio_queue;
	
	std::deque<SillyPtr>   sillys;
	std::deque<StaticPtr>  statics;
	std::deque<DynamicPtr> dynamics;
	std::deque<ShotPtr>    shots;
	
	friend class reaper::misc::UniquePtr<reaper::phys_dev::physEngine>;
};

}
}

#endif
