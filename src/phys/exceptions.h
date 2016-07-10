#ifndef REAPER_PHYS_EXCEPTIONS_H
#define REAPER_PHYS_EXCEPTIONS_H

#include "main/exceptions.h"
#include <string>

namespace reaper {
namespace phys {   
using std::string;

class phys_fatal_error
        : public fatal_error_base
{
protected:
        string err;
public:
        phys_fatal_error(const string &s) : err(s) {}
        const char* what() const { return err.c_str(); }
};

/// Non-fatal errors
class phys_error
        : public error_base
{   
protected:
        string err;
public:
        phys_error(const string &s) : err(s) {}
        const char* what() const { return err.c_str(); }                        
};

}
}

#endif
