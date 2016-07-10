

/* $Id: exceptions.h,v 1.1 2001/11/26 02:20:15 peter Exp $ */


#ifndef REAPER_OBJECT_EXCEPTIONS_H
#define REAPER_OBJECT_EXCEPTIONS_H

#include "main/exceptions.h"

namespace reaper
{
namespace object
{   


class object_error_t : public error_base { };
typedef error_tmpl<object_error_t> object_error;


}
}

#endif

