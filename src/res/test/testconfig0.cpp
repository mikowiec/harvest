
#include "os/compat.h"

#include <string>

#include "res/config.h"
#include "misc/test_main.h"


using namespace reaper;


int test_main()
{
	res::ConfigEnv conf("hw_gfx");
	derr << "->" << std::string(conf["fullscreen"]) << "<-\n";
	derr << "->" << std::string(conf["height"]) << "<-\n";
	derr << "->" << std::string(conf["nograb"]) << "<-\n";
	return 0;
}


