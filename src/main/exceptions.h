
/* $Id: exceptions.h,v 1.22 2002/09/29 12:35:52 pstrand Exp $ */

#ifndef REAPER_MAIN_EXCEPTIONS_H
#define REAPER_MAIN_EXCEPTIONS_H

#include <exception>
#include <string>

namespace reaper
{

//void log_exception(const std::string& msg);

/// All reaper-exceptions should be derived from this.
class error_base
{
public:
	virtual ~error_base();
	virtual const char* what() const = 0;
};


/** Fatal exceptions, i.e. non-recoverable,
 *  but which may still be handled to enable
 * a controlled shutdown..
 */
class fatal_error_base : public error_base { };


/// Exception template to ease implementation of own exceptions.

template<class T>
class error_tmpl : public T
{
	std::string msg;
public:
	error_tmpl(const std::string& m)
	 : msg(m)
	{ }
	const char* what() const {
		return msg.c_str();
	}
};


#define define_reaper_exception(name) \
	class name##_t : public error_base { }; \
	typedef error_tmpl<name##_t> name;

define_reaper_exception(reaper_error);

}
    
#endif


