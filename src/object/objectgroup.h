#ifndef REAPER_OBJECT_OBJECTGROUP_H
#define REAPER_OBJECT_OBJECTGROUP_H

#include "object/mkinfo.h"
#include <deque>

namespace reaper {
namespace object {

typedef std::deque<object::MkInfo> MkInfoVec;

class ObjectGroup {
public:
	MkInfoVec objs;
};

void init_loader(const std::string& d);

}
}


#endif
