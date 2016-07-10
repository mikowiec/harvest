
#ifndef REAPER_WORLD_WORLD_IMPL_H
#define REAPER_WORLD_WORLD_IMPL_H

#include "os/compat.h"

#include "world/world.h"

#include "world/exceptions.h"
#include "game/level.h"
#include "object/base.h"
#include "misc/hash.h"

namespace reaper {
namespace world {

using object::SillyPtr;
using object::StaticPtr;
using object::DynamicPtr;
using object::ShotPtr;


typedef quadtree::QuadTree<Triangle*> tri_container;
typedef quadtree::QuadTree<object::SillyPtr> si_container;
typedef quadtree::QuadTree<object::StaticPtr> st_container;
typedef quadtree::QuadTree<object::DynamicPtr> dyn_container;
typedef quadtree::QuadTree<object::ShotPtr> shot_container;

struct World_impl
{
	tri_container* tri_tree;
	si_container* si_tree;
	st_container* st_tree;
	dyn_container* dyn_tree;
	shot_container* shot_tree;
	std::vector<ShotPtr> delayed_add_shot;
	std::vector<object::ID> delayed_del;

	hash::hash_map<object::ID, SillyPtr> sillys;
	hash::hash_map<object::ID, StaticPtr> statics;
	hash::hash_map<object::ID, DynamicPtr> dynamics;
	hash::hash_map<object::ID, ShotPtr> shots;

	object::ID local_player;

	game::LevelInfo* level_info;

	std::string level_name;

	bool in_shutdown;

	World_impl(const std::string& level);
	World_impl(const game::LevelInfo& level);
	~World_impl();

	void init();

	void shutdown();

	void set_local_player(object::ID);
};

}
}


#endif


