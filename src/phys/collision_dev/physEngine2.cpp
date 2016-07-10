#include "os/compat.h"
#include "physEngine2.h"
#include "world/world.h"
#include "exceptions.h"

using reaper::phys::phys_error;
using reaper::phys::phys_fatal_error;
#include "main/types_ops.h"
#include "world/world.h"
#include "game/level.h"
#include "object/phys.h"
#include "misc/smartptr.h"
typedef reaper::misc::SmartPtr<reaper::phys_dev::Pair> pairPtr;

#include <iostream>
using std::cout;
using std::endl;
#include <algorithm>
#include <set>
using std::find;
using std::set;

using namespace reaper::world;

namespace {
	reaper::debug::DebugOutput dout("phys::physEngine", 5);
}

#include "main/types_io.h"

namespace reaper{
namespace phys_dev{

#include <time.h>

inline double stopwatch() {return (double)clock()/(double)CLOCKS_PER_SEC;}

physEngine::physEngine():
wr(reaper::world::World::get_ref()),
frame_stop(0),
simulating(false),
tri_interval(1.0),
since_last_tri(tri_interval+0.1)

{
	if(!wr.valid()) {
		throw phys_fatal_error("phys::Engine::Engine() World not created!");
	}
//	dout << "created\n";
	
	startup();
}

PhysRef physEngine::create()
{
	return PhysRef(new physEngine());
}

PhysRef physEngine::get_ref()
{
	return PhysRef();
}


void physEngine::update_world(double start_time, double delta_time)
{
	frame_stop = start_time + delta_time;
	simulating = true;

	since_last_tri += delta_time;

	if(since_last_tri > tri_interval){
		since_last_tri = 0.0;
		double tri_time = tri_interval - 0.1;  //Garanteed frame ratee 10fps virtual
		for(dyn_iterator i = wr->begin_dyn();i != wr->end_dyn(); ++i) {
			object::phys::ObjectAccessor &phys_acc = i->get_physics();
			
			float radius = phys_acc.radius;
			radius 
				+= length(i->get_velocity())*tri_time
				+ phys_acc.max_acc*pow(tri_time,2);
			
			tri_iterator tr(wr->find_tri(Sphere(i->get_pos(),radius)));
			
			for( ; tr != wr->end_tri(); ++tr){
				Pair* objtri = new DynTriPair(*i,*tr);
				objtri->calc_lower_bound();
				prio_queue.push(objtri);				
			}				
		}
	}


	if(!prio_queue.empty()) {
		while( !prio_queue.empty() && prio_queue.top()->get_lower_bound() < frame_stop ) {
			
			//Now check if we want to simulate a collision between objects                                 
			Pair* pair = prio_queue.top();
						
			double dt = pair->get_lower_bound() - pair->get_sim_time();
			
			if(dt < 0)
				cout << "Neg dt!" << endl;

			//the object will be simulated this far when the loop has ended

			prio_queue.pop();
			//Pair simulate simulates until lower_bound                                   
			pair->simulate( pair->get_lower_bound() );                                        
			
			if(pair->check_distance() < Collision_Distance ) {                                                
				pair->collide( *(pair->get_collision()) ); 
			}
						
			if( to_insert(frame_stop,*pair) ) {
				pair->calc_lower_bound();
				prio_queue.push(pair);				
			}
			else{
				delete pair;
			}
	

		}

	}
		
	//Now simulate all objects until framestop
	//Simulate all aobjekt so that they have a simtime of end of fram		
	for(dyn_iterator i(wr->begin_dyn()); i != wr->end_dyn(); ++i){
		double dt = frame_stop - i->get_sim_time();
		if(dt < 0 ) 
			cout << "Negative dt!" << dt <<endl;
		i->simulate(dt);
	}
	
	for(st_iterator i(wr->begin_st()); i != wr->end_st(); ++i){
		double dt = frame_stop - i->get_sim_time();
		i->simulate(dt);
	}
	
	for(shot_iterator i(wr->begin_shot()); i != wr->end_shot(); ++i){
		double dt = frame_stop - i->get_sim_time();
		i->simulate(dt);
	}

	simulating = false;
	
	//Inserting all objects
	for(std::deque<ShotPtr>::iterator i    = shots.begin();    i != shots.end(); ++i)    { insert(*i); }
	for(std::deque<SillyPtr>::iterator i   = sillys.begin();   i != sillys.end(); ++i)   { insert(*i); }
	for(std::deque<StaticPtr>::iterator i  = statics.begin();  i != statics.end(); ++i)  { insert(*i); }
	for(std::deque<DynamicPtr>::iterator i = dynamics.begin(); i != dynamics.end(); ++i) { insert(*i); }
	
	sillys.clear();
	statics.clear();
	dynamics.clear();
	shots.clear();
	
}

bool physEngine::to_insert(double framestop,const Pair& p)
{
	return true;  //Check if any object is dead?
}

void physEngine::startup()
{
	for(dyn_iterator dyn(wr->begin_dyn()); dyn != wr->end_dyn(); ++dyn) {
		dyn_iterator i(dyn);
		++i;
		for(; i != wr->end_dyn();++i) {
			DynDynPair* pair = new DynDynPair(*dyn, *i);
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}
	
	for(dyn_iterator dyn(wr->begin_dyn()); dyn != wr->end_dyn(); ++dyn) {
		for(st_iterator i(wr->begin_st()); i != wr->end_st(); ++i) {
			StaticDynPair* pair = new StaticDynPair(*i, *dyn);
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}
	
	for(dyn_iterator dyn(wr->begin_dyn()); dyn != wr->end_dyn(); ++dyn) {
		for(si_iterator i(wr->begin_si()); i != wr->end_si(); ++i) {
			SillyDynPair* pair = new SillyDynPair(*i, *dyn);
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}
}

void physEngine::insert(ShotPtr shot)
{
	if(simulating)
		shots.push_back(shot);
	else
		insert(shot,frame_stop);
	
}


void physEngine::insert(ShotPtr shot,double sim_time)
{
	const double& simt = shot->get_sim_time();
	
	shot->simulate(frame_stop - simt);
	shot->set_sim_time(frame_stop);
	
	//TEST to simulate shots here
	
	float dies_at = shot->get_sim_time();
	
	dies_at+= shot->get_lifespan(); 
	
	
	//Get all objects inside a frustum, exclude object which is at
	//a position near the shot, which should by the object that fired the 
	//shot
	
	
	wr->add_object(shot);
	
	using reaper::world::Line;
	using reaper::world::Sphere;
	using reaper::world::Frustum;
	
	const Point p1(shot->get_pos());
	const Point p2(shot->get_pos() + shot->get_velocity()*shot->get_lifespan());
	
	//Some geomterics for the object
	Line line(p1,p2);
	Sphere shot_sphere(shot->get_pos(), shot->get_radius() );
	Frustum dyn_frust(shot->get_pos(),
		shot->get_velocity()*(shot->get_lifespan() + 1),
		shot->get_mtx().col(1),
		45.0,
		45.0);
	
	
	reaper::world::dyn_iterator di(wr->find_dyn(dyn_frust)), 
		cur_di(wr->find_dyn(shot_sphere));
	
	
	for( ; di != wr->end_dyn(); ++di) {
		if(di != cur_di){
			Pair* pair = new ShotDynPair(shot, *di );
			pair->calc_lower_bound();
			
			if(pair->get_lower_bound() < dies_at )
				prio_queue.push(pair);
			else 
				delete pair; 
		}
	}
	
	reaper::world::st_iterator st(wr->find_st(line)),
		cur_st(wr->find_st(shot_sphere));
	
	
	for(; st != wr->end_st(); ++st) {
		if(st != cur_st){
			Pair* pair = new ShotSillyPair(shot, *st);
			pair->calc_lower_bound();
			
			if(pair->get_lower_bound() < dies_at)
				prio_queue.push(pair);
			else
				delete pair;
		}
	}
	
	
	reaper::world::si_iterator si(wr->find_si(line)),
		cur_si(wr->find_si(shot_sphere));
	
	
	for(; si != wr->end_si(); ++si) {
		if(si != cur_si){
			ShotSillyPair* pair = new ShotSillyPair(shot, *si);
			pair->calc_lower_bound();
			
			if(pair->get_lower_bound() < dies_at)
				prio_queue.push(pair);
			else
				delete pair;
		}
	}
	
	reaper::world::tri_iterator tri(wr->find_tri(line));
	
	for( ; tri != wr->end_tri(); ++tri){
		Pair* pair = new ShotTriPair(shot, *tri);
		pair->calc_lower_bound();			
		if(pair->get_lower_bound() < dies_at)
			prio_queue.push(pair);												
		else 
			delete pair;
	}
	
}

void physEngine::insert(PlayerPtr p)
{
	insert(DynamicPtr(p));
}

void physEngine::insert(DynamicPtr dyn)
{
	if(simulating){
		dynamics.push_back(dyn);
		return;
	}
	
	dyn->set_sim_time(frame_stop);
	
	
	//Create pairs with all existing dynamic, silly and static objects
	
	for(dyn_iterator i(wr->begin_dyn()); i != wr->end_dyn();++i){
		if(! length(dyn->get_pos()-i->get_pos()) - 
			(dyn->get_radius() + i-> get_radius() )
			< Collision_Distance){
			Pair* pair = new DynDynPair(dyn, *i);
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}
	
	
	for(st_iterator i(wr->begin_st()); i != wr->end_st(); ++i) {
		if(! length(dyn->get_pos()-i->get_pos()) - 
			(dyn->get_radius() + i-> get_radius() )
			< Collision_Distance){
			Pair* pair = new StaticDynPair(*i, dyn);
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}		
	
	
	for(si_iterator i(wr->begin_si()); i != wr->end_si(); ++i) {
		if(! length(dyn->get_pos()-i->get_pos()) - 
			(dyn->get_radius() + i-> get_radius() )
			< Collision_Distance){
			Pair* pair = new SillyDynPair(*i, dyn);
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}
	
	
	wr->add_object(dyn);
	
}

void physEngine::insert(SillyPtr sil)
{
	if(simulating){
		sillys.push_back(sil);
		return;
	}
	
	
	
	for(dyn_iterator i(wr->begin_dyn()); i != wr->end_dyn(); ++i) {
		if(! length(sil->get_pos()-i->get_pos()) - 
			(sil->get_radius() + i-> get_radius() )
			< Collision_Distance){
			Pair* pair = new SillyDynPair(sil, *i);
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}
	wr->add_object(sil);
}

void physEngine::insert(StaticPtr stat)
{
	if(simulating){
		statics.push_back(stat);
		return;
	}
	
	stat->set_sim_time(frame_stop);
	
	for(dyn_iterator i(wr->begin_dyn()); i!= wr->end_dyn(); ++i){
		if(! length(stat->get_pos()-i->get_pos()) - 
			(stat->get_radius() + i-> get_radius() )
			< Collision_Distance){
			Pair* pair = new StaticDynPair(stat,*i);
			pair->calc_lower_bound();
			prio_queue.push(pair);
		}
	}
	wr->add_object(stat);
}

int physEngine::size() const
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

void physEngine::shutdown()
{
	wr.release();
	dealloc(prio_queue);
	sillys.clear();
	statics.clear();
	dynamics.clear();
	shots.clear();
}

void physEngine::reinit()
{
	dout << "Rebuilding pairs\n";
	dealloc(prio_queue);
	startup();
}

physEngine::~physEngine()
{
	dout << "destroyed\n";
	dealloc(prio_queue);
//	PhysRef::destroy();
}


}//phys_dev
}//reaper


