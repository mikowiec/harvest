
#include "os/compat.h"

#include "misc/lua.h"

#include "misc/test_main.h"

#include "res/resource.h"
#include "res/res.h"

using namespace reaper;
using namespace misc;


int test_main()
{
	std::string msg = res::sanity_check();
	lua::Lua lua;
//	res::res_stream terrain2(res::File, "objects/shipX.luac");
	res::res_stream terrain2(res::File, "terrains/terrain2.luac");

	derr << lua.load("lua", terrain2) << "\n";
	derr << lua.pcall(0, -1, 0) << '\n';

	lua.get_global("shipX");
	derr << lua.type_name(lua.type(-1)) << '\n';


	return 0;
}


/*






 */
