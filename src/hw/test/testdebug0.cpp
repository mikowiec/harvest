

#include "os/compat.h"
#include "os/debug.h"


using namespace reaper::debug;

int main()
{
	try {
		DebugOutput derr1("test");
		DebugOutput derr2("test", 2);
		derr1 << "one" << std::endl;
		derr2 << "two" << std::endl;
		return 0;
	} catch (...) {
		return 1;
	}
}


