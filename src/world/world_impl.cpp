/* $Id: world_impl.cpp,v 1.17 2003/06/11 15:16:51 pstrand Exp $ */

#include "os/compat.h"

#include "world/world.h"

#include "os/debug.h"
#include "misc/free.h"
#include "misc/iostream_helper.h"
#include "misc/sequence.h"
#include "misc/stlhelper.h"
#include "object/base.h"
#include "object/factory.h"
#include "world/exceptions.h"
#include "game/level.h"
#include "world/world_impl.h"
#include "res/res.h"

#include <queue>


namespace reaper {
namespace { 
    reaper::debug::DebugOutput dlog("world", 5); 
}
namespace object { 
    int get_obj_count(); 
}
namespace world {       


using misc::lower_bound;
using misc::upper_bound;
using misc::seq;
using misc::for_each;
using misc::copy;
using misc::sort;
using std::mem_fun_ref;
using misc::Seq;

using object::SillyPtr;
using object::StaticPtr;
using object::DynamicPtr;
using object::ShotPtr;


namespace {
	object::ID next_id = 10000;
}

void reset_id()
{
	next_id = 10000;
}

object::ID alloc_id()
{
	return next_id++;
}




World_impl::World_impl(const std::string& level)
 : tri_tree(0), si_tree(0), st_tree(0), dyn_tree(0), 
   // sthold("world", this),
   level_name(level), in_shutdown(false)
{
	reset_id();
	init();
}


World_impl::~World_impl() {
//		dlog << "dyn world size: " << *dyn_objs.front().get().getcount().count
//		     << ' ' << dyn_objs.size() << '\n';
//		dlog << "shot world size: " << *dyn_objs.front().get().getcount().count
//		     << ' ' << shot_objs.size() << '\n';
//		for_each(seq(dyn_objs), mem_fun_ref(&Box<DynamicPtr>::dealloc));
//		for_each(seq(shot_objs), mem_fun_ref(&Box<ShotPtr>::dealloc));
	dlog << "~World_impl objects: " << object::get_obj_count() << '\n';
}

void World_impl::init()
{
	using std::max;
	level_info = new game::LevelInfo(level_name);
	float min_x = level_info->terrain_min_x;
	float min_z = level_info->terrain_min_z;
	float dim = max(level_info->terrain_max_x - level_info->terrain_min_x,
			level_info->terrain_max_x - level_info->terrain_min_x);

	tri_tree = new tri_container(min_x, min_z, dim);
	si_tree  = new si_container(min_x, min_z, dim);
	st_tree  = new st_container(min_x, min_z, dim);
	dyn_tree = new dyn_container(min_x, min_z, dim);
	shot_tree = new shot_container(min_x, min_z, dim);

}

class ostream_out
{
	std::ostream& os;
public:
	ostream_out(std::ostream& o) : os(o) { }
	template<class T>
	void operator()(const T& t)
	{
		t->dump(os);
		os << '\n';
	}

};

void print_it(std::pair<object::ID, DynamicPtr> p)
{
	dlog << p.first << ' ' << p.second.valid() << '\n';
}

void World_impl::set_local_player(object::ID id)
{
//	dlog << " set lp, size: " << dynamics.size() << '\n';

//	for_each(seq(dynamics), print_it);

	local_player = id;

/*	
	DynamicPtr d = dynamics[id];
	if (!d.valid())
		throw world_error("get_local_player: not found");
	local_player.dynamic_assign(d);
	if (!local_player.valid())
		throw world_error("get_local_player: not a player object!");
*/
}

void World_impl::shutdown()
{
	in_shutdown = true;

	delayed_add_shot.clear();
	dlog << "delayed shots: " << delayed_add_shot.size() << '\n';
	for_each(seq(*tri_tree), misc::delete_it);

	misc::zero_delete(tri_tree);
	misc::zero_delete(st_tree);
	misc::zero_delete(si_tree);
	misc::zero_delete(dyn_tree);
	misc::zero_delete(shot_tree);
	misc::zero_delete(level_info);
	sillys.clear();
	statics.clear();
	dynamics.clear();
	dlog << "shots: " << shots.size() << '\n';
	shots.clear();
	dlog << "objects after shutdown: " << object::get_obj_count() << '\n';
}



}

}
