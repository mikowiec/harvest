// bucket.cpp: implementation of the bucket class.
//
//////////////////////////////////////////////////////////////////////

#include "bucket.h"
#include "exceptions.h"

using reaper::phys::phys_error;

bucket_key::bucket_key(int x, int y, int z): x(x),y(y),z(z)
{
        if( x < 0 || y < 0 || z < 0)
                throw phys_error("Bucket_Key: Negative key!");
        
}


bucket::bucket(const bucket_key &k):
key(k) 
{
}

bucket::~bucket()
{ 
}

/////////////////////

bool bucket_key::operator <(const bucket_key& rhs) const
{
        if(x == rhs.x)
                if(y == rhs.y)
                        return z < rhs.z;
                else
                        return y < rhs.y;
        else
                return x < rhs.x;
}



bool bucket::operator ==(const bucket& rhs) const
{
        return key == rhs.key;
}

bool bucket::operator <(const bucket& rhs) const
{
        return key < rhs.key;
}


void bucket::set_key(const bucket_key &k) 
{
        key = k;
}

bool bucket::has_key(const bucket_key &k) const
{
        return key == k;
}
