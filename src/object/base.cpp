
#include "os/compat.h"

#include <string>

#include "object/base.h"

#include "main/types_io.h"

namespace reaper {
namespace object {

// from object/object.cpp
void obj_count_incr(Base*);
void obj_count_decr(Base*);
int get_obj_count();


Base::Base()
{
	obj_count_incr(this);
}
Base::~Base()
{
	obj_count_decr(this);
}



}
}


