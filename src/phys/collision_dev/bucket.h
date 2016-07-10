// bucket.h: interface for the bucket class.
//
//////////////////////////////////////////////////////////////////////

#ifndef BUCKET_H
#define BUCKET_H

#include "os/compat.h"

#include <vector>
#include <set>
#include <iostream>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef int objId;

using std::set;

class bucket_key 
{
public: 
        int x,y,z;
        bucket_key(int x, int y, int z);
        bool operator==(const bucket_key& rhs) const { return x==rhs.x && y==rhs.y && z == rhs.z;}
        bool operator <(const bucket_key& rhs) const;
};

class bucket  
{
public:
	bucket(const bucket_key&);
        ~bucket();
        bool operator==(const bucket&) const;
        bool operator<(const bucket&) const;
        void set_key(const bucket_key&) ;
        const bucket_key & get_key() const {return key;}
        bool has_key(const bucket_key&) const;

        set<objId> objects;

private:
        bucket_key key;

};

#endif // BUCKET_H
