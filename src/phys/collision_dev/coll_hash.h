// coll_hash.h: interface for the coll_hash class.
//
//////////////////////////////////////////////////////////////////////

#ifndef COLL_HASH_H
#define COLL_HASH_H

#include "os/compat.h"
#include "misc/smartptr.h"
#include "main/types_ops.h"

using reaper::Point;

#include <list>
#include <vector>
#include <functional>
#include <algorithm>

#include "bucket.h"

using std::list;
using std::vector;

#define largest_prime 16

typedef reaper::misc::SmartPtr<bucket> buckPtr;
typedef std::list<buckPtr> buckList;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class buck_eq: public std::unary_function<buckPtr,bool> 
{
        bucket_key k;
public:
        explicit buck_eq(const bucket_key& kk) :k(kk) {}
        bool operator() (const buckPtr& b) {return b->has_key(k);}
};



class coll_hash  
{
public:
        coll_hash();
	~coll_hash();
        
        void remove(const objId id,const bucket_key&);
        void insert(const objId id,const bucket_key&);
        const set<objId>& get_ids(const bucket_key&) const;
	void print();  //temporary 
        vector<bucket_key> get_keys();
private:

        int hash_func(const bucket_key&) const;

        //A list of sizes for the hash_table

        const static int primes[largest_prime];
        int current_size;

        int num_elems;
        std::vector<buckList> elements;

};

class grid
{
public:
        grid(float minX, float minY, float minZ, float distance);
        ~grid() {};
        void calc_key(const Point& p1,const Point& p2, vector<bucket_key> &) const;
        void get_points(const bucket_key&,Point& min,Point& max) const;

private:
        float mX,mY,mZ;
        float distance;
};

#endif // COLL_HASH_H
