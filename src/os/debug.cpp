/* $Id: debug.cpp,v 1.2 2003/01/06 12:42:12 pstrand Exp $ */

#include "os/compat.h"

#include "os/debug.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <deque>
#include <algorithm>

#include "os/abstime.h"

#include "misc/sequence.h"

namespace reaper
{
namespace debug
{


namespace {

struct DebugData {
	int cur_prio;
	std::set<std::string> disabled_id;
	bool enable;
	DebugData() : cur_prio(0), enable(true) { }
};

DebugData& dd() {
	// FIXME
	static DebugData dp;
	return dp;
}


}


void debug_priority(int p)
{
	dd().cur_prio = p;
}

void debug_disable(const std::string& name)
{
	dd().disabled_id.insert(name);
}

void debug_disable()
{
	dd().enable = false;
}


DebugStream::DebugStream(const std::string& id, int pri)
 : dbgfile(0), ident(id), prio(pri)
{
	if (dd().enable) {
		static std::ofstream df("debug.out", std::ios::out | std::ios::trunc);
		dbgfile = &df;
		setp(0, 0);
	}
}

typedef std::char_traits<char> ct;

bool DebugStream::is_enabled(const std::string& id, int pri)
{
	return (pri <= dd().cur_prio) &&
	       (dd().disabled_id.find(id) == dd().disabled_id.end());
}

ct::int_type DebugStream::overflow(ct::int_type c)
{
	if (!dd().enable)
		return ct::eof();

	current_line += ct::to_char_type(c);

	if (ct::to_char_type(c) == '\n') {
		std::ostringstream msg;

		msg << '[' << os::time::strtime("") << "] "
		    << ident << '(' << prio << ") "
		    << current_line;

		*dbgfile << msg.str() << std::flush;
		if (is_enabled(ident, prio)) {
			std::cerr << msg.str();
		}
		current_line.erase();
	}
	return ct::not_eof(c);
}

DebugOutput::DebugOutput(const std::string& id, int pri)
 : std_ostream(0)
 , dbg_stream(id, pri)
{
	init(new DebugStream(id, pri));
}


DebugOutput::~DebugOutput()
{
	delete rdbuf();
	init(0);
}

}
}


