
#include "os/compat.h"

#include "os/debug.h"

#include "main/types_ops.h"
#include "misc/free.h"
#include "misc/parse.h"
#include "misc/sequence.h"
#include "misc/stlhelper.h"
#include "object/accessor.h"
#include "object/base.h"
#include "object/factory.h"
#include "world/exceptions.h"
#include "game/level.h"
#include "world/world.h"
#include "world/geometry.h"
#include "world/world_impl.h"

#include <queue>


namespace reaper {
namespace { 
        reaper::debug::DebugOutput dlog("world", 5); 
}
namespace misc {
	UniquePtr<world::World>::I UniquePtr<world::World>::inst;
}
namespace object { int get_obj_count(); }
namespace world
{       

using std::string;

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

Triangle::Triangle(const Point &p0, const Point &p1, const Point &p2)
 : a(p0),b(p1),c(p2)
{
	edges[0] = b - a;
	edges[1] = c - b;
	edges[2] = a - c;

	normal = norm(cross(edges[0],edges[1]));

	for (int i = 0; i < 3; i++)
		norms[i] = norm(cross(edges[i],normal));
}

Frustum::Frustum(const Point& p, const Vector& d, const Vector& u,
		 float fw, float fh)
 : pos_(p), dir_(d),
   up_(norm(u)),
   left_(norm(cross(up_, dir_))),
   fov_w(fw), fov_h(fh), 
   cut(length(dir_)),
   radius(cut / (cos(3.14/180*fov_h/2) * cos(3.14/180*fov_w/2)) )
{ }

typedef quadtree::QuadTree<Triangle*> tri_container;
typedef quadtree::QuadTree<object::SillyPtr> si_container;
typedef quadtree::QuadTree<object::StaticPtr> st_container;
typedef quadtree::QuadTree<object::DynamicPtr> dyn_container;
typedef quadtree::QuadTree<object::ShotPtr> shot_container;


World::World()
 : impl(0)
{ }

void World::load(const string& lv)
{
	impl = new World_impl(lv);
}


void World::shutdown()
{
	dlog << "shutdown\n";
	if (impl) {
		impl->shutdown();
		delete impl;
		impl = 0;
	}
}

World::~World() 
{
	shutdown();
	dlog << "World destroyed\n";
}

template<class S>
void run_mem(S s) {
	while (s) {
		s->key = calc_key(*s);
		++s;
	}
}



}
}

/*
namespace {

template <class Iter, class T>
inline void linear_insert(Iter first, Iter last, T)
{
	T val = *last;
	if (val < *first) {
		copy_backward(first, last, last + 1);
		*first = val;
	}
	else
	{
		Iter next = last;
		--next;
		while (val < *next) {
			*last = *next;
			last = next;
			--next;
		}
		*last = val;
	}
}

template <class Iter>
void isort(Iter first, Iter last)
{
  if (first == last) return; 
  for (Iter i = first + 1; i != last; ++i)
    linear_insert(first, i, *first);
}

}
*/

//namespace std {

/*
typedef reaper::world::SearchCont<reaper::object::DynamicPtr>::iterator Iter1;
void sort(Iter1 b, const Iter1 & e) { isort(b, e); }

typedef reaper::world::SearchCont<reaper::object::ShotPtr>::iterator Iter2;
void sort(Iter2 b, const Iter2 & e) { isort(b, e); }
*/


//}

namespace reaper {
namespace world {

void World::update(bool all)
{
	for (int i = 0; i < impl->delayed_add_shot.size(); ++i) {
		ShotPtr o = impl->delayed_add_shot[i];
		impl->shot_tree->insert(o);
		impl->shots[o->stat().id()] = o;
	}
	impl->delayed_add_shot.clear();
//	printf("\nBefore\n");
//	impl->dyn_tree->show();
	impl->dyn_tree->update();
//	impl->dyn_tree->show();
	impl->shot_tree->update();
//	printf("After\n");
	if (all) {
		impl->tri_tree->update();
		impl->si_tree->update();
		impl->st_tree->update();
	}

//	run_mem(seq(impl->shot_objs));
//	sort(seq(impl->shot_objs));
	delayed_del();
}


// blä, usch, urk, tvi... 

tri_iterator World::begin_tri() { return impl->tri_tree->begin(); }
tri_iterator World::end_tri()   { return impl->tri_tree->end(); }
tri_iterator World::find_tri(const Sphere& s)   { return impl->tri_tree->find(s); }
tri_iterator World::find_tri(const Cylinder& s) { return impl->tri_tree->find(s); }
tri_iterator World::find_tri(const Line& s)     { return impl->tri_tree->find(s); }
tri_iterator World::find_tri(const Frustum& s)  { return impl->tri_tree->find(s); }
tri_iterator World::find_tri(const Rect& s)     { return impl->tri_tree->find(s); }

st_iterator World::begin_st() { return impl->st_tree->begin(); }
st_iterator World::end_st()   { return impl->st_tree->end(); }
st_iterator World::find_st(const Sphere& s)   { return impl->st_tree->find(s); }
st_iterator World::find_st(const Cylinder& s) { return impl->st_tree->find(s); }
st_iterator World::find_st(const Line& s)     { return impl->st_tree->find(s); }
st_iterator World::find_st(const Frustum& s)  { return impl->st_tree->find(s); }
st_iterator World::find_st(const Rect& s)     { return impl->st_tree->find(s); }

si_iterator World::begin_si() { return impl->si_tree->begin(); }
si_iterator World::end_si()   { return impl->si_tree->end(); }
si_iterator World::find_si(const Sphere& s)   { return impl->si_tree->find(s); }
si_iterator World::find_si(const Cylinder& s) { return impl->si_tree->find(s); }
si_iterator World::find_si(const Line& s)     { return impl->si_tree->find(s); }
si_iterator World::find_si(const Frustum& s)  { return impl->si_tree->find(s); }
si_iterator World::find_si(const Rect& s)     { return impl->si_tree->find(s); }

dyn_iterator World::begin_dyn() { return impl->dyn_tree->begin(); }
dyn_iterator World::end_dyn()   { return impl->dyn_tree->end(); }
dyn_iterator World::find_dyn(const Sphere& s)   { return impl->dyn_tree->find(s); }
dyn_iterator World::find_dyn(const Cylinder& s) { return impl->dyn_tree->find(s); }
dyn_iterator World::find_dyn(const Line& s)     { return impl->dyn_tree->find(s); }
dyn_iterator World::find_dyn(const Frustum& s)  { return impl->dyn_tree->find(s); }
dyn_iterator World::find_dyn(const Rect& s)     { return impl->dyn_tree->find(s); }

shot_iterator World::begin_shot() { return impl->shot_tree->begin(); }
shot_iterator World::end_shot()   { return impl->shot_tree->end(); }
shot_iterator World::find_shot(const Sphere& s)   { return impl->shot_tree->find(s); }
shot_iterator World::find_shot(const Cylinder& s) { return impl->shot_tree->find(s); }
shot_iterator World::find_shot(const Line& s)     { return impl->shot_tree->find(s); }
shot_iterator World::find_shot(const Frustum& s)  { return impl->shot_tree->find(s); }
shot_iterator World::find_shot(const Rect& s)     { return impl->shot_tree->find(s); }




float World::get_altitude(const Cylinder& cyl) 
{
	return impl->tri_tree->max_height(cyl);
}

float World::get_altitude(const Rect& r) 
{
	return impl->tri_tree->max_height(r);
}

float dot2(const Point2D& p1, const Point2D& p2)
{
	return p1.x*p2.x + p1.y*p2.y;
}

float World::get_altitude(const Point2D& p) 
{
	Cylinder cyl(p, 0.01);
	tri_iterator tri = impl->tri_tree->find(cyl); // inder(p, 0.01));
	if (tri == impl->tri_tree->end())
		return 0;

	Triangle* tri2 = *tri;
	do {
		Point pt = proj_on_tri(**tri, p);
		if (intersect(**tri,pt))
			return pt.y;
		++tri;
	} while (tri != impl->tri_tree->end());
	dlog << "world integrity failure!\n";
	return tri2->a.y;
}
void World::set_local_player(object::ID id)
{
	impl->set_local_player(id);
}

template<class iterator, class W>
iterator find_id(W* w, object::ID id)
{
	iterator c, e = iterator();
	for (c = iterator(w); c != e; ++c) {
		if ((*c)->stat().id() == id)
			return c;
	}
	return e;
}

template<class iterator, class W>
bool erase_id(W* w, object::ID id)
{
	iterator i = find_id<iterator>(w, id);
	if (i == iterator())
		return false;
	i.erase();
	return true;
}

DynamicPtr World::get_local_player()
{
	DynamicPtr dp = lookup_dyn(impl->local_player);
	return dp;
}

const game::LevelInfo& World::get_level_info() const
{
	if (!impl->level_info)
		throw world_nonexistant("get_level_info");
	return *impl->level_info;
}



void World::add_object(Triangle* o)
{
	impl->tri_tree->insert(o);
}


void World::add_object(SillyPtr o)
{
	impl->si_tree->insert(o);
	impl->sillys[o->stat().id()] = o;
}

void World::add_object(StaticPtr o)
{
	impl->st_tree->insert(o);
	impl->statics[o->stat().id()] = o;
}

void World::add_object(DynamicPtr o)
{
	impl->dyn_tree->insert(o);
	impl->dynamics[o->stat().id()] = o;
}

void World::add_object(ShotPtr o)
{
//	impl->delayed_add_shot.push_back(o);
		impl->shot_tree->insert(o);
		impl->shots[o->stat().id()] = o;
}


SillyPtr   World::lookup_si(object::ID id)   { return impl->sillys[id]; }
StaticPtr  World::lookup_st(object::ID id)   { return impl->statics[id]; }
DynamicPtr World::lookup_dyn(object::ID id)  { return impl->dynamics[id]; }
ShotPtr    World::lookup_shot(object::ID id) { return impl->shots[id]; }


template<class iterator, class W, class B>
void erase_obj_q(W* w, B o) {
	iterator c, e = w->end();
	for (c = w->begin(); c != e; ++c) {
		if (*c == o) {
			w->erase(c);
			return;
		}
	}
}

template<class iterator, class W, class B>
void erase_obj_s(W* w, B o) {
	iterator c, e = iterator();
	for (c = iterator(w); c != e; ++c) {
		if ((*c).get() == o) {
			c.erase();
			return;
		}
	}
}


void World::erase(SillyPtr o)    { if (! impl->in_shutdown) { impl->sillys.erase(o->stat().id());   erase_obj_q<si_iterator>(impl->si_tree, o); } }
void World::erase(StaticPtr o)   { if (! impl->in_shutdown) { impl->statics.erase(o->stat().id());  erase_obj_q<st_iterator>(impl->st_tree, o); } }
void World::erase(DynamicPtr o)  { if (! impl->in_shutdown) { impl->dynamics.erase(o->stat().id()); erase_obj_q<dyn_iterator>(impl->dyn_tree, o); } }
void World::erase(ShotPtr o)     { if (! impl->in_shutdown) { impl->shots.erase(o->stat().id());    erase_obj_q<shot_iterator>(impl->shot_tree, o); } }


void World::erase(tri_iterator i)  { impl->tri_tree->erase(i); }
void World::erase(st_iterator i)   { impl->sillys.erase((*i)->stat().id()); impl->st_tree->erase(i); }
void World::erase(si_iterator i)   { impl->statics.erase((*i)->stat().id()); impl->si_tree->erase(i); }
void World::erase(dyn_iterator i)  { impl->dynamics.erase((*i)->stat().id()); impl->dyn_tree->erase(i); }
void World::erase(shot_iterator i) { impl->shots.erase((*i)->stat().id()); impl->shot_tree->erase(i); }

bool World::erase(object::ID id) {
	/* We must delay complete deletion to keep objects
	 * alive during a cycle.
	 */
	impl->delayed_del.push_back(id);
	return erase_id<shot_iterator>(impl->shot_tree, id) ||
	       erase_id<si_iterator>(impl->si_tree, id)     ||
	       erase_id<st_iterator>(impl->st_tree, id)     ||
	       erase_id<dyn_iterator>(impl->dyn_tree, id);
}

void World::delayed_del()
{
	for (int i = 0; i < impl->delayed_del.size(); ++i) {
		object::ID id = impl->delayed_del[i];
		impl->shots.erase(id);
		impl->sillys.erase(id);
		impl->statics.erase(id);
		impl->dynamics.erase(id);
	}
	impl->delayed_del.clear();
}

int World::silly_size() const { return impl->sillys.size(); }
int World::static_size() const { return impl->statics.size(); }
int World::dynamic_size() const { return impl->dynamics.size(); }
int World::shot_size() const { return impl->shots.size(); }






}

}
