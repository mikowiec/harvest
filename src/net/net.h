#ifndef REAPER_NET_NET_H
#define REAPER_NET_NET_H

#include <iostream>
#include <set>
#include <map>
#include <deque>
#include <string>

#include "main/types.h"
#include "os/reltime.h"
#include "object/base.h"

namespace reaper {
namespace net {


typedef std::string IntStateRep;
typedef std::deque<IntStateRep> StateReps;

struct StateRep
{
	object::ID id;
	os::time::RelTime sync;
	IntStateRep rep;
};

std::ostream& operator<<(std::ostream&, const StateRep&);
std::istream& operator>>(std::istream&, StateRep&);

typedef std::map<object::ID, StateRep> ObjStateMap;

typedef os::time::RelTime Timestamp;


typedef int GameState;
const int init = 0;
const int active = 1;
const int startup = 2;
const int running = 4;


}
}

#endif


