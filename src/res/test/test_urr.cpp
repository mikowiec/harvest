
#include "os/compat.h"
#include "os/debug.h"
#include "res/urr.h"
#include "misc/lua.h"

#include <stack>

#include "misc/test_main.h"

#include <iostream>


namespace reaper {
namespace res {
namespace urr {

using namespace misc::lua;
	
class LuaLurker : public Reader, public Writer
{
public:
	Lua& lua;
	std::stack<int> ts;
	bool at_end;
	LuaLurker(Lua& l, int);
	~LuaLurker();
	bool next();
	bool descend();
	bool descend(int);
	bool descend(std::string);
	bool up();


	bool read(float&d);
	bool read(std::string&);
	int read_binary(unsigned char* buf, int n);

	bool write(std::string);
	bool write_binary(unsigned char* buf, int n);
};

class LuaBrowser : public Urr
{
public:
	Lua lua;
	int tbl;
	LuaBrowser(const std::string&);
	~LuaBrowser();

	Reader* reader();
	Writer* writer();

};
}
}
}

using namespace reaper::res::urr;
using namespace std;


int test_main()
{
	LuaBrowser* b = new LuaBrowser("levels/level1");
	res::urr::LuaLurker* rd = new LuaLurker(b->lua, b->tbl);
	string s;
	
	derr << rd->lua.top() << '\n';
	if (rd->descend("sky")) {
		if (rd->descend("texture")) {
			rd->read(s);
			derr << "sky_texture " << s << '\n';
			rd->up();
		}
		rd->up();
	}
	derr << rd->lua.top() << '\n';
	rd->descend("lakes");
	rd->descend();
	do {
		rd->read(s);
		derr << "lake     " << rd->lua.top() << ' ' << s << '\n';
	} while (rd->next());
	rd->up();
	rd->up();
	derr << "mid   " << rd->lua.top() << '\n';
	do {
		rd->read(s);
		derr << rd->lua.top() << ' ' << s << '\n';
		if (rd->descend()) {
			do {
				rd->read(s);
				derr << "     " << rd->lua.top() << ' ' << s << '\n';
			} while (rd->next());
			derr << "    " << rd->lua.top() << '\n';
			rd->up();
			derr << "    " << rd->lua.top() << '\n';
		}
	} while (rd->next());
	derr << "mid " << rd->lua.top() << '\n';
	rd->descend("lakes");
	rd->descend();
	do {
		rd->read(s);
		derr << "lake     " << rd->lua.top() << ' ' << s << '\n';
	} while (rd->next());
	rd->up();
	rd->up();
	derr << "end " << rd->lua.top() << '\n';
	return 0;
}
