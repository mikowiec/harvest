
#include "os/compat.h"


#include "phys/engine.h"
#include "phys/engine_impl.h"

#include "world/world.h"
#include "phys/exceptions.h"

#include "main/types_ops.h"
#include "world/world.h"
#include "game/level.h"
#include "misc/smartptr.h"
#include "misc/sequence.h"
#include "misc/free.h"
#include "misc/profile.h"

#include "main/types_io.h"
#include "object/phys.h"
#include "object/accessor.h"

#include <iostream>
#include <algorithm>
#include <set>

#include <time.h>
#include <iomanip>

using reaper::phys::phys_error;
using reaper::phys::phys_fatal_error;

using std::cout;
using std::endl;
using std::find;
using std::set;

using namespace reaper::world;

namespace reaper {
        typedef reaper::misc::SmartPtr<reaper::phys::Pair> pairPtr;
namespace {
	debug::DebugOutput dout("phys::Engine", 5);
}

namespace phys {


Engine::Engine()
 : i(new impl::Engine())
{
}

Engine::~Engine()
{
	delete i;
}

void Engine::update_world(double start_time, double delta_time)
{
	i->update_world(start_time, delta_time);
}

void Engine::insert(PrimAccessor* prim)
{
	i->insert(prim);
}

void Engine::insert(SimAccessor* sim)
{
	i->insert(sim);
}

void Engine::insert(DynAccessor* dyn)
{
	i->insert(dyn);
}

void Engine::insert(ShotAccessor* shot)
{
	i->insert(shot);
}

void Engine::insert(ShotAccessor* shot,double sim_time)
{
	i->insert(shot, sim_time);
}

void Engine::remove(objId id)
{
	i->remove(id);
}

int Engine::size()
{
	return i->size();
}

void Engine::shutdown()
{
	i->shutdown();
}

namespace impl {

Pair* make_pair(DynAccessor* d, world::Triangle* t)  { return new DynTriPair(d, t); }
Pair* make_pair(DynAccessor* d1, DynAccessor* d2)    { return new DynDynPair(d1, d2); }
Pair* make_pair(DynAccessor* d, SimAccessor* s)      { return new StaticDynPair(s, d); }
Pair* make_pair(DynAccessor* d, PrimAccessor* p)     { return new SillyDynPair(p, d); }

Pair* make_pair(ShotAccessor* d, world::Triangle* t) { return new ShotTriPair(d, t); }
Pair* make_pair(ShotAccessor* s, DynAccessor* d)     { return new ShotDynPair(s, d); }
Pair* make_pair(ShotAccessor* s, PrimAccessor* p)    { return new ShotSillyPair(s, p); }

using misc::for_each;
using misc::seq;
using misc::delete_it;




inline double stopwatch() {return (double)clock()/(double)CLOCKS_PER_SEC;}

Engine::Engine():
frame_stop(0),
simulating(false),
tri_interval(1.0),
since_last_tri(tri_interval+0.1),
last_garbage(0.0)
{
//	dout << "created\n";
	
	startup();
}



template<class T>
std::mem_fun1_t<int, Engine, T*> mk_ins_fun()
{
	return std::mem_fun1_t<int,Engine,T*>((int (Engine::*)(T*))&Engine::insert);
}

using misc::time::Profile;

void Engine::update_world(double start_time, double delta_time)
{
	static misc::time::Profiler p1,p2,p3,p4,p5,p6;
	reaper::world::WorldRef wr;

	frame_stop = start_time + delta_time;
	simulating = true;

	since_last_tri += delta_time;
	{Profile p(p1);
	for(dyn_iterator i = wr->begin_dyn();i != wr->end_dyn(); ++i) {
		object::DynamicPtr d = *i;
		
		DynAccessorPtr dacc = dynamics_map[d->stat().id()];

		float max_acc = dacc->max_acc();
		float radius = dacc->get_radius();

		radius += length(dacc->get_velocity())*delta_time
			+ max_acc*pow(delta_time,2);
		
		tri_iterator tr(wr->find_tri(Sphere(dacc->get_mtx().pos(),radius)));
		
		for( ; tr != wr->end_tri(); ++tr){
			Pair* objtri = make_pair(dacc.get(), *tr);
			objtri->calc_lower_bound();
			if(objtri->get_lower_bound() < frame_stop)
				prio_queue.push(objtri);
			else 
				delete objtri;
			
		}				
	}
	}
	int ic = 0, ri = 0, rni = 0;
	float sum_dt = 0;
	if(!prio_queue.empty()) {
	{Profile p(p2);

		while( !prio_queue.empty() && prio_queue.top()->get_lower_bound() < frame_stop ) {
//			if (ic > 10)
//				dout << "big ic\n";
			Profile p(p4);
			
			//Now check if we want to simulate a collision between objects                                 
			Pair* pair = prio_queue.top();

			//the object will be simulated this far when the loop has ended

			prio_queue.pop();
			{Profile p(p3);
			//Pair simulate simulates until lower_bound 
			if(dead_objects.find(pair->get_id1()) != dead_objects.end() || 
				dead_objects.find(pair->get_id2()) != dead_objects.end() ){
				delete pair;
			}
			else {

				pair->simulate( pair->get_lower_bound() );                                        

				double dist = pair->check_distance();
				if(pair->check_distance() < Collision_Distance ) {
					CollisionData* cd = pair->get_collision();
					pair->collide(cd);
					delete cd;
				}				

				pair->calc_lower_bound();
				double lb = pair->get_lower_bound();
				if(pair->to_insert(frame_stop)  ) {
//					dout << "reinsert " << pair->get_id1() << "  with dist: " << pair->check_distance() << '\n';
					prio_queue.push(pair);
					sum_dt += fabs(frame_stop - pair->get_lower_bound());
					ri++;
					if (pair->get_lower_bound() < frame_stop)
						rni++;
				} else
					delete pair;
			}
			}
			ic++;
		}

	}
	}
	
	//Now simulate all objects until framestop
	//Simulate all aobjekt so that they have a simtime of end of fram		

	//Pair simulate simulates until lower_bound 

	for(dyn_iterator i(wr->begin_dyn()); i != wr->end_dyn(); ++i){
		if (is_dead((*i)->stat().id()))
			continue;
		DynAccessorPtr p = dynamics_map[(*i)->stat().id()];
		double dt = frame_stop - p->get_sim_time();
		if(dt > 0)
			p->simulate(dt);
	}
	
	for(st_iterator i(wr->begin_st()); i != wr->end_st(); ++i){
		if (is_dead((*i)->stat().id()))
			continue;
		SimAccessorPtr p = statics_map[(*i)->stat().id()];
		double dt = frame_stop - p->get_sim_time();
		if(dt > 0)
			p->simulate(dt);
	}
	
	for(shot_iterator i(wr->begin_shot()); i != wr->end_shot(); ++i){
		ShotAccessorPtr p = shots_map[(*i)->stat().id()];
		/* 
		 * FIXME: p should always be valid here!
		 * (it is probably due to shots which are 
		 * not properly killed. see their physaccimpl..
		 */
		if (!p.valid() || is_dead(p->get_id()))
			continue;
		double dt = frame_stop - p->get_sim_time();
		if(dt > 0) {
			p->simulate(dt);
		}
	}

	simulating = false;
	//Inserting all objects

	for_each(seq(sillys), std::bind1st(mk_ins_fun<PrimAccessor>(), this));
	for_each(seq(statics), std::bind1st(mk_ins_fun<SimAccessor>(), this));
	for_each(seq(dynamics), std::bind1st(mk_ins_fun<DynAccessor>(), this));
	for_each(seq(shots), std::bind1st(mk_ins_fun<ShotAccessor>(), this));

	if(last_garbage + Maximum_Lower_bound < frame_stop){
		clean_dead_objects();
		last_garbage = frame_stop;
	}

	sillys.clear();
	statics.clear();
	dynamics.clear();
	shots.clear();

}


bool Engine::is_dead(objId id) {
	return !(dead_objects.find(id) == dead_objects.end());
}

void Engine::remove(objId id)
{
	dead_objects[id] = frame_stop + Maximum_Lower_bound;
//	sillys_map[id] = statics_map[id] = dynamics_map[id] = shots_map[id] = 0;
}

void Engine::clean_dead_objects()
{
	int i = 0, j = 0;
	hash::hash_map<objId,double>::iterator tmp, it = dead_objects.begin();

	while(it != dead_objects.end() ){
		++i;
		tmp = it++;
		if(tmp->second < frame_stop) {
			++j;
			objId id = tmp->first;
			dead_objects.erase(id);
			sillys_map[id] = 0;
			statics_map[id] = 0;
			dynamics_map[id] = 0;
			shots_map[id] = 0;
		}
	}
}

bool Engine::to_insert(double framestop,const Pair& p)
{
	return true;  //Check if any object is dead?
}

void Engine::startup()
{
	reaper::world::WorldRef wr;
	for(dyn_iterator dyn(wr->begin_dyn()); dyn != wr->end_dyn(); ++dyn) {
		DynAccessorPtr da = dynamics_map[(*dyn)->stat().id()];

		for (dyn_iterator i(dyn); i != wr->end_dyn(); ) {
			++i;
			Pair* pair = make_pair(da.get(), dynamics_map[(*i)->stat().id()].get());
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}

		for (st_iterator i(wr->begin_st()); i != wr->end_st(); ++i) {
			Pair* pair = make_pair(da.get(), statics_map[(*i)->stat().id()].get());
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}

		for (si_iterator i(wr->begin_si()); i != wr->end_si(); ++i) {
			Pair* pair = make_pair(da.get(), sillys_map[(*i)->stat().id()].get());
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}
}


template<class PairType, class Prio, class Iter, class M>
void create_pairs(Prio& prio_queue, DynAccessor* dyn, Iter begin, Iter end, M& map)
{
	for (Iter i = begin; i != end; ++i) {
		if (! length(dyn->get_mtx().pos() - (*i)->stat().pos())
		      - (dyn->get_radius() + (*i)->stat().radius())
		     < Collision_Distance) {
			Pair* pair = make_pair(dyn, map[(*i)->stat().id()].get());
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}
}


int Engine::insert(ShotAccessor* shot)
{
	if(simulating)
		shots.push_back(shot);
	else
		insert(shot,frame_stop);
	return 0;	
}


int Engine::insert(ShotAccessor* shot,double sim_time)
{
	reaper::world::WorldRef wr;
//	const double& simt = shot->get_sim_time();
	
	//shot->simulate(frame_stop - simt);

	shot->set_sim_time(frame_stop);
	
	//TEST to simulate shots here

	shots_map[shot->get_id()] = ShotAccessorPtr(shot);

	float dies_at = shot->get_sim_time();
	
	dies_at += shot->get_lifespan(); 
	
	
	//Get all objects inside a frustum

	using reaper::world::Line;
	using reaper::world::Frustum;
	
	const Point p1 = shot->get_mtx().pos();
	const Point p2(shot->get_mtx().pos() + shot->get_velocity()*shot->get_lifespan());
	
	//Some geomterics for the object
	Line line(p1,p2);
	Frustum dyn_frust(shot->get_mtx().pos(),
			  shot->get_velocity()*(shot->get_lifespan() + 1),
			  shot->get_mtx().col(1),
			  45.0, 45.0);

	object::ID parent = shot->get_parent();

	world::dyn_iterator di(wr->find_dyn(dyn_frust));

	for( ; di != wr->end_dyn(); ++di) {
		if((*di)->stat().id() != parent) {
			Pair* pair = make_pair(shot, dynamics_map[(*di)->stat().id()].get());
			pair->calc_lower_bound();

			if(pair->get_lower_bound() < dies_at )
				prio_queue.push(pair);
			else 
				delete pair; 
		}
	}
	
	world::st_iterator st(wr->find_st(line));

	for(; st != wr->end_st(); ++st) {
		if((*st)->stat().id() != parent) {
			Pair* pair = make_pair(shot, statics_map[(*st)->stat().id()].get());
			pair->calc_lower_bound();
			
			if(pair->get_lower_bound() < dies_at)
				prio_queue.push(pair);
			else
				delete pair;
		}
	}
	
	
	world::si_iterator si(wr->find_si(line));


	for (; si != wr->end_si(); ++si) {
		if ((*si)->stat().id() != parent){
			Pair* pair = make_pair(shot, sillys_map[(*si)->stat().id()].get());
			pair->calc_lower_bound();
			
			if(pair->get_lower_bound() < dies_at)
				prio_queue.push(pair);
			else
				delete pair;
		}
	}
	
	world::tri_iterator tri(wr->find_tri(line));

	for( ; tri != wr->end_tri(); ++tri){
		ShotTriPair* pair = new ShotTriPair(shot, *tri);
		double d1 = pair->check_distance();
		pair->calc_lower_bound();
		float lb = pair->get_lower_bound();
		if (d1 > 0 && lb < dies_at) {
			prio_queue.push(pair);												
		} else {
			delete pair;
		}
	}

	return 0;
}

int Engine::insert(DynAccessor* dyn)
{
	reaper::world::WorldRef wr;
	if(simulating){
		dynamics.push_back(dyn);
		return 0;
	}

	dyn->set_sim_time(frame_stop);

	dynamics_map[dyn->get_id()] = DynAccessorPtr(dyn);

	create_pairs<DynDynPair>(prio_queue, dyn, wr->begin_dyn(), wr->end_dyn(), dynamics_map);
	create_pairs<StaticDynPair>(prio_queue, dyn, wr->begin_st(), wr->end_st(), statics_map);
	create_pairs<SillyDynPair>(prio_queue, dyn, wr->begin_si(), wr->end_si(), sillys_map);

	return 0;
}

int Engine::insert(PrimAccessor* sil)
{
	reaper::world::WorldRef wr;
	if(simulating){
		sillys.push_back(sil);
		return 0;
	}

	sillys_map[sil->get_id()] = PrimAccessorPtr(sil);

//	create_pairs<SillyDynPair>(prio_queue, sil, wr->begin_dyn(), wr->end_dyn(), dynamics_map);
	for(dyn_iterator i(wr->begin_dyn()); i != wr->end_dyn(); ++i) {
		if(! length(sil->get_mtx().pos() - (*i)->stat().pos()) - 
			(sil->get_radius() + (*i)->stat().radius() )
			< Collision_Distance){
			Pair* pair = make_pair(dynamics_map[(*i)->stat().id()].get(), sil);
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}
	return 0;
}

int Engine::insert(SimAccessor* stat)
{
	reaper::world::WorldRef wr;
	if(simulating){
		statics.push_back(stat);
		return 0;
	}

	stat->set_sim_time(frame_stop);

	statics_map[stat->get_id()] = SimAccessorPtr(stat);

	for(dyn_iterator i(wr->begin_dyn()); i!= wr->end_dyn(); ++i){
		if(! length(stat->get_mtx().pos() - (*i)->stat().pos()) - 
			(stat->get_radius() + (*i)->stat().radius() )
		     < Collision_Distance){
			Pair* pair = make_pair(dynamics_map[(*i)->stat().id()].get(), stat);
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}
//	wr->add_object(stat);
	return 0;
}

int Engine::size()
{
	return prio_queue.size();
}

template<class P>
void dealloc(P& pairs)
{
	while(!pairs.empty()) {
		Pair* p = pairs.top();
		delete p;
		pairs.pop();
	}
}

void Engine::shutdown()
{
	dealloc(prio_queue);
	for_each(seq(sillys), delete_it);   sillys.clear();
	for_each(seq(statics), delete_it);  statics.clear();
	for_each(seq(dynamics), delete_it); dynamics.clear();
	for_each(seq(shots), delete_it);    shots.clear();
	sillys_map.clear();
	statics_map.clear();
	dynamics_map.clear();
	shots_map.clear();
}

void Engine::reinit()
{
	dout << "Rebuilding pairs\n";
	dealloc(prio_queue);
	startup();
}

Engine::~Engine()
{
	dout << "destroyed\n";
	dealloc(prio_queue);
//	PhysRef::destroy();
}

} // impl
} // phys
} // reaper


