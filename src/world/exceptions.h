
/* $Id: exceptions.h,v 1.18 2002/12/29 17:04:43 pstrand Exp $ */


#ifndef REAPER_WORLD_EXCEPTIONS_H
#define REAPER_WORLD_EXCEPTIONS_H 

#include "os/compat.h"
#include "main/exceptions.h"

namespace reaper
{
namespace world
{

define_reaper_exception(world_init_error);
define_reaper_exception(world_existing);
define_reaper_exception(world_nonexistant);
define_reaper_exception(world_format_error);
define_reaper_exception(world_error);

}
}

#endif

