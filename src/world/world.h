#ifndef REAPER_WORLD_WORLD_H
#define REAPER_WORLD_WORLD_H

#include "main/types_forward.h"
#include "object/base.h"
#include "misc/uniqueptr.h"
#include "world/query_obj.h"

#include "world/worldref.h"
#include "world/quadtree.h"


namespace reaper
{

namespace game {
	class LevelInfo;
}
/// %World information (%terrain and all objects)
namespace world
{


object::ID alloc_id();


struct World_impl;

typedef quadtree::QuadTree<Triangle*>::iterator tri_iterator;
typedef quadtree::QuadTree<object::SillyPtr>::iterator si_iterator;
typedef quadtree::QuadTree<object::StaticPtr>::iterator st_iterator;
typedef quadtree::QuadTree<object::DynamicPtr>::iterator dyn_iterator;
typedef quadtree::QuadTree<object::ShotPtr>::iterator shot_iterator;

/// World class
class World : public misc::singleton::Base
{
	World_impl* impl;

	friend class misc::UniquePtr<World>;
	void delayed_del();
public:

	void load(const std::string& lv);

	// prepare for shutdown
	// (remove all object, unallocate as much as possible)
	void shutdown();


	World();

	~World();

	void update(bool all = false);

	// blä, usch, urk, tvi... 

	tri_iterator begin_tri();
	tri_iterator end_tri();
	tri_iterator find_tri(const Sphere& s);
	tri_iterator find_tri(const Cylinder& s);
	tri_iterator find_tri(const Line& s);
	tri_iterator find_tri(const Frustum& s);
	tri_iterator find_tri(const Rect& s);

	si_iterator begin_si();
	si_iterator end_si();
	si_iterator find_si(const Sphere& s);
	si_iterator find_si(const Cylinder& s);
	si_iterator find_si(const Line& s);
	si_iterator find_si(const Frustum& s);
	si_iterator find_si(const Rect& s);
	object::SillyPtr lookup_si(object::ID);

	st_iterator begin_st();
	st_iterator end_st();
	st_iterator find_st(const Sphere& s);
	st_iterator find_st(const Cylinder& s);
	st_iterator find_st(const Line& s);
	st_iterator find_st(const Frustum& s);
	st_iterator find_st(const Rect& s);
	object::StaticPtr lookup_st(object::ID);

	dyn_iterator begin_dyn();
	dyn_iterator end_dyn();
	dyn_iterator find_dyn(const Sphere& s);
	dyn_iterator find_dyn(const Cylinder& s);
	dyn_iterator find_dyn(const Line& s);
	dyn_iterator find_dyn(const Frustum& s);
	dyn_iterator find_dyn(const Rect& s);
	object::DynamicPtr lookup_dyn(object::ID);

	shot_iterator begin_shot();
	shot_iterator end_shot();
	shot_iterator find_shot(const Sphere& s);
	shot_iterator find_shot(const Cylinder& s);
	shot_iterator find_shot(const Line& s);
	shot_iterator find_shot(const Frustum& s);
	shot_iterator find_shot(const Rect& s);
	object::ShotPtr lookup_shot(object::ID);

	void add_object(Triangle* o);
	void add_object(object::ShotPtr o);
	void add_object(object::DynamicPtr o);
	void add_object(object::StaticPtr o);
	void add_object(object::SillyPtr o);

	void erase(tri_iterator);
	void erase(si_iterator);
	void erase(st_iterator);
	void erase(dyn_iterator);
	void erase(shot_iterator);

	void erase(object::ShotPtr);
	void erase(object::DynamicPtr);
	void erase(object::StaticPtr);
	void erase(object::SillyPtr);

	bool erase(object::ID id);

	int static_size() const;
	int silly_size() const;
	int dynamic_size() const;
	int shot_size() const;


	/// Returns the altitude at the specified point.
	float get_altitude(const Point2D& p);
	/// Returns (approximately) the altitude within a specified area.
	float get_altitude(const Cylinder& cyl);
	float get_altitude(const Rect& r);

	void set_local_player(object::ID id);

	/// Returns local player
	/*!
	  \return Pointer to the local player object,
	  	  or null, should none exist.
	 */
	object::DynamicPtr get_local_player();

	const game::LevelInfo& get_level_info() const;

};

}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2003/06/12 08:17:36 $
 * $Log: world.h,v $
 * Revision 1.107  2003/06/12 08:17:36  pstrand
 * switch to loose, non-sharing quadtree
 *
 * Revision 1.106  2003/06/11 15:16:51  pstrand
 * småfixar..
 *
 * Revision 1.105  2003/06/04 17:17:49  pstrand
 * Decoupling of objects and subsystems, and a number of minor updates and cleanups
 *
 * Revision 1.104  2002/04/29 20:43:20  pstrand
 * shots i quadtree, upprensning&buggfixar..
 *
 * Revision 1.103  2002/04/12 18:51:13  pstrand
 * dynobj i quadtree
 *
 * Revision 1.102  2002/04/11 01:17:49  pstrand
 * Changed to more explicit handling of refptr/uniqueptrs. Destruction is not automatic.
 * RefPtrs are replaced by UniquePtrs.
 * Further cleanups are possible.
 *
 * Revision 1.101  2002/03/24 17:40:06  pstrand
 * no message
 *
 * Revision 1.100  2002/03/19 20:38:49  pstrand
 * modified for netplay.., and a few simplifications
 *
 * Revision 1.99  2002/03/18 13:34:09  pstrand
 * rect search & level fix
 *
 * Revision 1.98  2002/02/18 11:52:07  peter
 * obj.ladd. och reorg.
 *
 * Revision 1.97  2002/01/17 04:57:54  peter
 * errmsg fix.. mm..
 *
 * Revision 1.96  2002/01/01 23:32:21  peter
 * alloc_id och level
 *
 * Revision 1.95  2001/12/02 17:40:50  peter
 * objmgrmeck
 *
 * Revision 1.94  2001/11/21 00:52:10  peter
 * destroy test
 *
 * Revision 1.93  2001/11/10 13:57:06  peter
 * minnesfixar...
 *
 * Revision 1.92  2001/08/27 12:55:33  peter
 * objektmeck...
 *
 * Revision 1.91  2001/08/20 17:11:46  peter
 * obj.ptr
 *
 * Revision 1.90  2001/08/06 12:16:47  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.89.2.3  2001/08/05 00:11:11  peter
 * döda...
 *
 * Revision 1.89.2.2  2001/08/03 13:44:17  peter
 * pragma once obsolete...
 *
 * Revision 1.89.2.1  2001/08/02 16:29:15  peter
 * objfix...
 *
 * Revision 1.89  2001/07/27 15:46:21  peter
 * hmm.. reason for const? (get_local_player)
 *
 * Revision 1.88  2001/07/11 10:56:37  peter
 * uppdateringar (strul med gcc3 och optimeringar!)
 *
 * Revision 1.87  2001/07/09 13:33:13  peter
 * gcc-3.0 fixar
 *
 * Revision 1.86  2001/07/06 01:47:39  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.85  2001/05/12 22:06:09  peter
 * *** empty log message ***
 *
 * Revision 1.84  2001/05/11 18:51:43  peter
 * *** empty log message ***
 *
 *
 */

