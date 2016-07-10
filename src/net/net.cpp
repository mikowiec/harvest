
#include "os/compat.h"

#include <iostream>

#include "main/types_io.h"
#include "net/net.h"
#include "misc/iostream_helper.h"

namespace reaper {
namespace net {

using misc::spaces;


std::ostream& operator<<(std::ostream& os, const StateRep& rep)
{
	os << rep.id << ' ' << std::hex << rep.sync << ' '
	   << rep.rep;
	return os;
}

	
std::istream& operator>>(std::istream& is, StateRep& rep)
{
	is >> rep.id >> spaces >> std::hex >> rep.sync >> spaces;
	misc::until(is, rep.rep, "\r\n");
	return is;
}



}
}

