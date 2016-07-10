
/* $Id: exceptions.h,v 1.2 2003/01/06 12:42:12 pstrand Exp $ */


#ifndef REAPER_HW_EXCEPTIONS_H
#define REAPER_HW_EXCEPTIONS_H

#include "main/exceptions.h"

namespace reaper
{
namespace os
{   

class hw_fatal_error_t : public fatal_error_base { };
typedef error_tmpl<hw_fatal_error_t> hw_fatal_error;

class hw_error_t : public error_base { };
typedef error_tmpl<hw_error_t> hw_error;


}
}

#endif

