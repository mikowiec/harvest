#ifndef PHYSOBJECT_H
#define PHYSOBJECT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "coll_hash.h"
#include "misc/hash.h"
#include <vector>

using std::vector;

class physObject  
{
public:
	physObject();
	virtual ~physObject();
	
	void update_keys(double& dt);
	const vector<bucket_key>& new_keys();
	const vector<bucket_key>& bad_keys();



protected:
	reaper::misc::hash_map< int, vector< bucket_key > > oldkeys;
};

#endif // !PHYSOBJECT_H
