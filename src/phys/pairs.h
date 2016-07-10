#ifndef REAPER_PHYS_PAIRS_H
#define REAPER_PHYS_PAIRS_H

#include "world/triangle.h"
//#include "world/world.h"
#include "main/types.h"
#include "phys/physConstants.h"
#include "phys/accessor.h"

#include <vector>


using reaper::world::Triangle;
using std::vector;
using reaper::Point;

namespace reaper {
namespace phys {

class CollisionData
{
public:
	CollisionData(const Point &p) : pos(p) {}
	Point pos;
	Vector norm1;
	Vector norm2;
};

typedef int objId;

class Pair {
public:
	Pair();
	
	virtual ~Pair();
	
	virtual void calc_lower_bound() = 0;
	virtual double check_distance() = 0;
	virtual CollisionData* get_collision() = 0;
	virtual void simulate(const double& to_time) = 0;
	virtual bool collide(const CollisionData*) = 0;
	virtual double get_sim_time() const = 0;
	inline bool has_id(objId id) const { return id1==id || id2 == id;}

	inline double get_lower_bound() const { return lower_bound; }
	
	virtual bool operator<(const Pair& rh) const;
	inline  bool operator>(const Pair& rh) const { return ! (*this < rh || *this == rh);}
	virtual bool operator==(const Pair& rh) const { return id1 == rh.id1 && id2 == rh.id2;}


	inline bool less_than(const Pair& rh) const { return lower_bound < rh.lower_bound; };
	inline bool larger_than(const Pair& rh) const { return lower_bound > rh.lower_bound; };
	virtual bool to_insert(const double& framestop) {return true;};

	inline objId get_id1() const { return id1;}
	inline objId get_id2() const { return id2;}

protected:

	double lower_bound;
	objId id1;
	objId id2;
};

class DynTriPair : public Pair
{
public:
	DynTriPair(DynAccessor* i1,Triangle* t) : Pair(),obj(i1),tri(t) 
	{
		id1 = obj->get_id();
	};
	
	virtual ~DynTriPair() { }
	
	virtual void calc_lower_bound() ;
	virtual double check_distance() ;
	
	virtual CollisionData* get_collision();
	virtual void simulate(const double& to_time) ;
	virtual bool collide(const CollisionData*);
	virtual double get_sim_time() const {return obj->get_sim_time();}
	virtual bool has_id(objId id) const { return id1==id;}
	bool to_insert(const double& fs);


private:
	DynAccessor* obj;
	Triangle* tri;
};

class ShotTriPair : public Pair
{
public:
	ShotTriPair(ShotAccessor* i1,Triangle* t) : Pair(),obj(i1),tri(t) 
	{
		id1 = obj->get_id();
	};
	
	virtual ~ShotTriPair() { }
	
	virtual void calc_lower_bound();
	virtual double check_distance();	
	virtual CollisionData* get_collision();
	virtual void simulate(const double& to_time);
	virtual bool collide(const CollisionData*);
	virtual double get_sim_time() const {return obj->get_sim_time();}
	virtual bool has_id(objId id) const { return id1==id;}
	bool to_insert(const double& fs);

private:
	ShotAccessor* obj;
	Triangle* tri;
};



class DynDynPair : public Pair {
public:
	DynDynPair(DynAccessor* obj1, DynAccessor* obj2) : Pair(),
							obj1(obj1), obj2(obj2) 
	{
		id1 = obj1->get_id();
		id2 = obj2->get_id();
	}

	double get_sim_time() const;
	void calc_lower_bound();
	virtual double check_distance() ;
	CollisionData* get_collision();
	void simulate(const double& to_time);
	bool collide(const CollisionData*);

private:
	DynAccessor* obj1;
	DynAccessor* obj2;
};



class SillyDynPair : public Pair {
public:
	SillyDynPair(PrimAccessor* obj1, DynAccessor* obj2) 
		: Pair(),obj1(obj1), obj2(obj2) 
	{
		id1 = obj1->get_id();
		id2 = obj2->get_id();
	}

	double get_sim_time() const;
	void calc_lower_bound();
	CollisionData* get_collision();
	virtual void simulate(const double &to_time);
	bool collide(const CollisionData*);

	virtual double check_distance() ;

protected:
	PrimAccessor* obj1;
	DynAccessor* obj2;
};

class StaticDynPair : public SillyDynPair {
public:
	StaticDynPair(SimAccessor* obj1, DynAccessor* obj2)
	 : SillyDynPair(obj1,obj2), obj1(obj1) {}
	void simulate(const double& to_time);
protected:
	SimAccessor* obj1;
};

class ShotSillyPair : public Pair {
public:
	ShotSillyPair(ShotAccessor* obj1, PrimAccessor* obj2) : shot(obj1), obj(obj2) {
		id1 = obj1->get_id();
		id2 = obj2->get_id();
	}

	virtual double get_sim_time() const;
	virtual void calc_lower_bound();
	CollisionData* get_collision();
	virtual void simulate(const double& to_time);
	bool collide(const CollisionData*);
	double check_distance();
protected:
	ShotAccessor* shot;
private:
	PrimAccessor* obj;
};

class ShotStaticPair : public ShotSillyPair{
public:
        ShotStaticPair(ShotAccessor* obj1, SimAccessor* obj2) : 
                ShotSillyPair(obj1,obj2), obj(obj2) {}
        void simulate(const double& to_time);
        virtual double get_sim_time() const;
private:
        SimAccessor* obj;
};

class ShotDynPair : public ShotStaticPair{
public:
        ShotDynPair(ShotAccessor* obj1, DynAccessor* obj2) : 
                ShotStaticPair(obj1,obj2), obj(obj2) {}
        void calc_lower_bound();
private:
        DynAccessor* obj;
};

}
}

#endif


