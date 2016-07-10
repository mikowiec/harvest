#ifndef PHYS_PHYSENGINE_H
#define PHYS_PHYSENGINE_H

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
namespace phys{

class Engine;
typedef reaper::misc::UniquePtr<reaper::phys::Engine> PhysRef;

class Engine  
{
public:
	~Engine();

	void update_world(double start_time, double delta_time);

	//void insert(ShotPtr shot);
	//void insert(ShotPtr shot,double sim_time);
	//void insert(PlayerPtr ply);
	//void insert(DynamicPtr dyn);
	//void insert(SillyPtr sil);
	//void insert(StaticPtr stat);
	//void tInsert( const testObj& );

	void remove(objId id);

	static PhysRef create();
	static PhysRef get_ref();


private:
	
	Engine();

	bool to_insert(double framestop,const Pair& p);
	void startup();

	reaper::world::WorldRef wr;
	
	double frame_stop;

	hash_map< objId, vector<bucket_key> >  old_keys;
	close_counter close_pairs;
	coll_hash collision_table;
	grid the_grid;
	CollisionQueue  prio_queue;
	
	
	friend class reaper::misc::UniquePtr<reaper::phys::Engine>;
};

}
}

#endif
