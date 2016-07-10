
/* $Id: debug.h,v 1.1 2002/12/29 16:58:14 pstrand Exp $ */


#ifndef HW_DEBUG_H
#define HW_DEBUG_H

#include <iostream>
#include <fstream>
#include <string>
#include <set>

namespace reaper
{
namespace debug
{


/** Filter based on priority. Used to disable all 
 *  DebugOutput generators with a lower priority.
 *  \param p New priority.
 */
void debug_priority(int p);


/** Filter based on name. Used to disable all
 *  DebugOutput generators with a specific name.
 */
void debug_disable(const std::string& name);

void debug_disable();

class DebugStream
 : public std::streambuf
{
	friend void debug_priority(int);
	friend void debug_disable(const std::string&);

	std::string current_line;
	std::ofstream* dbgfile;
	char buf[100];
	std::string ident;
	int prio;
public:
	typedef std::char_traits<char> ct;
	DebugStream(const std::string& id, int pri);
	ct::int_type overflow(ct::int_type c);

	static bool is_enabled(const std::string& id, int pri);
};


typedef std::ostream std_ostream;

/** Debug output. 
    Everything written to this will either be
    printed to a file or a window, or both.
    Every printed line is prepended with an
    identifier, current time and the priority.
 */

class DebugOutput
 : public std_ostream
{
	DebugStream dbg_stream;
public:
	/** Debugoutput generator. Used as an output stream, 
	 *  filters the output according to current priority.
	 *  0 is default, everything higher will be shown.
	 *  \param id Identifier.
	 *  \param pri Priority (for filtering)
	 */
	DebugOutput(const std::string& id = "", int pri = 0);
	~DebugOutput();

	void enable();
	void disable();
};




class ExitFail
{
	int rc;
	bool enabled;
public:
	ExitFail(int c) : rc(c), enabled(true) { }
	void disable()
	{
		enabled = false;
	}
	~ExitFail()
	{
		if (enabled)
			exit(rc);
	}
};

}
}


#endif

