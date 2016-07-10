#include "os/compat.h"
#include "physEngine.h"
#include "world/world.h"
#include "exceptions.h"

using reaper::phys::phys_error;
using reaper::phys::phys_fatal_error;
#include "main/types_ops.h"
#include "world/world.h"
#include "game/level.h"
#include "object/phys.h"
#include "misc/smartptr.h"
typedef reaper::misc::SmartPtr<reaper::phys::Pair> pairPtr;

#include <iostream>
using std::cout;
using std::endl;
#include <algorithm>
#include <set>
using std::find;
using std::set;

using namespace reaper::world;


#include "main/types_io.h"

namespace reaper{
namespace phys{


#include <time.h>

inline double stopwatch() {return (double)clock()/(double)CLOCKS_PER_SEC;}


typedef std::vector<bucket_key> bucVec;

Engine::Engine():
wr(reaper::world::World::get_ref()),
frame_stop(0),
the_grid(wr->get_level_info().terrain_min_x,
	 wr->get_level_info().terrain_min_x,
	 wr->get_level_info().terrain_min_z, 500)  //100 meter grids
{
	if(!wr.valid()) {
		throw phys_fatal_error("phys::Engine::Engine() World not created!");
	}
//	dout << "created\n";
	
	startup();
}

PhysRef Engine::create()
{
	return PhysRef(new Engine());
}

PhysRef Engine::get_ref()
{
	return PhysRef();
}

Engine::~Engine()
{

}

void Engine::update_world(double start_time, double delta_time)
{
	frame_stop = start_time + delta_time;

	double start = stopwatch();
	
	bucVec keys;
	bucVec new_keys;
	bucVec obsolete_keys;
	

	//Update the hashboxes for all dynamic objects
	for(dyn_iterator dyn_it = wr->begin_dyn();dyn_it != wr->end_dyn(); ++dyn_it){
	
		reaper::object::phys::ObjectAccessor &phys_acc = dyn_it->get_physics();
		
		const double len = length(phys_acc.vel)*delta_time;

		const double ext1 = phys_acc.radius + len + 
			 phys_acc.max_acc * delta_time* delta_time / 2;
		
		const double ext2 = ext1 - 2* len;

		const Point &pos = dyn_it->get_pos(); 
		
		Point pt1( pos + norm(phys_acc.vel)*ext1 );
		Point pt2( pos - norm(phys_acc.vel)*ext2 );

		const Point p1(min(pt1.x,pt2.x),min(pt1.y,pt2.y), min(pt1.z,pt2.z) );
		const Point p2(max(pt1.x,pt2.x), max(pt1.y,pt2.y), max(pt1.z,pt2.z) );

		the_grid.calc_key(p1,p2,keys);		
		
		bucVec& current_k = old_keys[dyn_it->get_id()];
				
		for(int i = 0;i < keys.size(); ++i){
			bucVec::const_iterator p = 
				find(current_k.begin(), current_k.end(), keys[i] );
			
			if(p == current_k.end() )
				new_keys.push_back(keys[i]);
		}
		
		for(int i = 0;i < current_k.size(); ++i){
			bucVec::const_iterator p2 = 
				find(keys.begin(), keys.end(), current_k[i] );
			
			if(p2 == keys.end() )
				obsolete_keys.push_back(keys[i]);
		}

		for(int i = 0;i < new_keys.size();++i){
			collision_table.insert(dyn_it->get_id(),new_keys[i]);
			const set<int>& ids = collision_table.get_ids(new_keys[i]);
			for(std::set<int>::const_iterator j = ids.begin();j != ids.end(); ++j){
				int res = close_pairs.increase(dyn_it->get_id(),*j);
				if(res == 1){
					Pair* pr;
					SillyPtr si = wr->lookup_si(*j);
					StaticPtr st = wr->lookup_st(*j);
					DynamicPtr dyn = wr->lookup_dyn(*j);
					ShotPtr sh = wr->lookup_shot(*j);
					if( si.valid() ){
						pr = new SillyDynPair(si,*dyn_it);
					}
					else if( st.valid() )
						pr = new StaticDynPair(st,*dyn_it);
					else if( dyn.valid() )
						pr = new DynDynPair(dyn,*dyn_it);
					else if( sh.valid() )
						pr = new ShotDynPair(sh,*dyn_it);
					pr->calc_lower_bound();
					prio_queue.insert(pr);
				}
			}
		}
		
		
		
		for(int i = 0;i < obsolete_keys.size();++i){
			collision_table.remove(dyn_it->get_id(),obsolete_keys[i]);
			const set<int> ids = collision_table.get_ids(new_keys[i]);
			for(std::set<int>::const_iterator j = ids.begin();j != ids.end(); ++j){
				close_pairs.decrease(dyn_it->get_id(),*j);
			}
		}

		

		new_keys.clear();
		obsolete_keys.clear();

	}

	double mid = stopwatch();
	
	if(!prio_queue.empty()) {
		while( !prio_queue.empty() && prio_queue.top()->get_lower_bound() < frame_stop ) {
			
			//Now check if we want to simulate a collision between objects                                 
			Pair* pair = prio_queue.top();
			
			double dt = pair->get_lower_bound() - pair->get_sim_time();
			
			if(dt < 0)
				cout << "Neg dt!" << endl;

			//the object will be simulated this far when the loop has ended

			//prio_queue.pop();
			//Pair simulate simulates until lower_bound                                   
			pair->simulate( pair->get_lower_bound() );                                        
			
			if(pair->check_distance() < Collision_Distance ) {                                                
				if (pair->collide( *(pair->get_collision()) ) ) 
					prio_queue.update_if(tstId(pair->get_id1(), pair->get_id2() ));
			}
						
			if( to_insert(frame_stop,*pair) ) {
				pair->calc_lower_bound();
				prio_queue.update(pair);
				
			}
			else{
				prio_queue.pop();
				delete pair;
			}
	

		}

	}
		

	double mid2 = stopwatch();

	//Now simulate all objects until framestop
	//Simulate all aobjekt so that they have a simtime of end of fram
	
	
	for(dyn_iterator i(wr->begin_dyn()); i != wr->end_dyn(); ++i){
		double dt = frame_stop - i->get_sim_time();
		if(dt < 0 ) 
			cout << "Negative dt!" << dt <<endl;
		i->simulate(dt);
	}

	double end = stopwatch();

	cout << "Timing info Hash: " << mid - start << " treap: " << mid2 - mid << endl;

	/*
	for(st_iterator i(wr->begin_st()); i != wr->end_st(); ++i){
		double dt = frame_stop - i->get_sim_time();
		i->simulate(dt);
	}
	
	for(shot_iterator i(wr->begin_shot()); i != wr->end_shot(); ++i){
		double dt = frame_stop - i->get_sim_time();
		i->simulate(dt);
	}

	*/
	
	
}

bool Engine::to_insert(double framestop,const Pair& p)
{
	return true;
}

void Engine::startup()
{
	bucVec keys;
	
	for(si_iterator si_it = wr->begin_si();si_it != wr->end_si(); ++si_it){
			
		const double &ext = si_it->get_radius();
		
		const Point &pos = si_it->get_pos(); 
		
		Point p1( pos.x-ext, pos.y-ext, pos.z-ext );
		Point p2( pos.x+ext, pos.y+ext, pos.z+ext );
		
	
		the_grid.calc_key(p1,p2,keys);
				
		for(int i = 0;i < keys.size();++i){
			collision_table.insert(si_it->get_id(),keys[i]);
			
		}

	}

	for(st_iterator st_it = wr->begin_st();st_it != wr->end_st(); ++st_it){
			
		const double &ext = st_it->get_radius();
		
		const Point &pos = st_it->get_pos(); 
		
		Point p1( pos.x-ext, pos.y-ext, pos.z-ext );
		Point p2( pos.x+ext, pos.y+ext, pos.z+ext );
		
		the_grid.calc_key(p1,p2,keys);
				
		for(int i = 0;i < keys.size();++i){
			collision_table.insert(st_it->get_id(),keys[i]);

		}

	}


	
}

}//phys
}//reaper


