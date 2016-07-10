#ifndef REAPER_GFX_EXCEPTIONS_H
#define REAPER_GFX_EXCEPTIONS_H

#include "main/exceptions.h"
#include <string>

namespace reaper {
namespace gfx {   
using std::string;

/**@name Exceptions - base classes */
//@{
/// Fatal errors
class gfx_fatal_error
        : public fatal_error_base
{
protected:
        string err;
public:
        gfx_fatal_error(const string &s) : err(s) {}
        const char* what() const { return err.c_str(); }
};

/// Non-fatal errors
class gfx_error
        : public error_base
{   
protected:
        string err;
public:
        gfx_error(const string &s) : err(s) {}
        const char* what() const { return err.c_str(); }                        
};
//@}

/// Thrown if interface creation fails
class gfx_init_error
        : public gfx_fatal_error
{
public:
        gfx_init_error(const string &s) : gfx_fatal_error(s) {}
};

/// Handles openGL error states
class gl_error
        : public gfx_fatal_error
{
protected:
        int err_num;
public:
	static const bool *test_for_errors;

        gl_error(int e, const string &s) : gfx_fatal_error(s), err_num(e) {}
        int err() const { return err_num; }
};

void throw_on_gl_error_check(const string &s);

/// Checks gl error status and throws exception if an error has occured
/** @param s message that is prepended to the gl error explanation
*/
inline void throw_on_gl_error(const string &s) {
	if(*gl_error::test_for_errors) {
		throw_on_gl_error_check(s);
	}
}

struct AssertGL {
	const std::string info;
	AssertGL(const std::string &i) : info(*gl_error::test_for_errors ? i : "" ) {
		throw_on_gl_error(info + " begin");
	}
	~AssertGL() {
		throw_on_gl_error(info + " end");
	}
};

}
}
#endif

/*
 * $Author: pstrand $
 * $Date: 2002/02/26 22:41:17 $
 * $Log: exceptions.h,v $
 * Revision 1.27  2002/02/26 22:41:17  pstrand
 * mackefix
 *
 * Revision 1.26  2002/01/28 00:50:38  macke
 * gfx reorg
 *
 * Revision 1.25  2001/12/17 16:28:31  macke
 * div bök
 *
 * Revision 1.24  2001/08/06 12:16:11  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.23.4.1  2001/08/03 13:43:51  peter
 * pragma once obsolete...
 *
 * Revision 1.23  2001/07/06 01:47:09  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.22  2001/06/09 01:58:46  macke
 * Grafikmotor reorg
 *
 * Revision 1.21  2001/05/10 11:40:11  macke
 * häpp
 *
 */

