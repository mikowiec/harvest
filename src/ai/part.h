
#ifndef REAPER_AI_PART_H
#define REAPER_AI_PART_H

#include "misc/command.h"

namespace reaper {
namespace ai {



class Part {

public:
	virtual void think() = 0;
	virtual ~Part() { }
};


misc::Ping reg(Part* p);

}
}

#endif

