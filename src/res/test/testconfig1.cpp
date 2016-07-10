
#include "os/compat.h"

#include <string>

#include "res/config.h"
#include "res/resource.h"
#include "misc/test_main.h"


using namespace reaper;


int test_main()
{
	res::ConfigEnv& conf = res::resource<res::ConfigEnv>("config/res_config_test");

	conf["pi"] = 3.14f;

	conf["foo_bar"] = 1;
	conf["foo_bar1"] = 2;
	conf["foo_bar2"] = 3;
	conf["foo_bar3"] = 4;
	conf["foo_bar4"] = 5;
	conf["foo_bar5"] = 6;
	

	res::save<res::ConfigEnv>("config/res_config_test");
	res::mgr().flush();

	derr << "pi = " << float(res::ConfigEnv("res_config_test")["pi"]) << '\n';;

	return 0;
}


