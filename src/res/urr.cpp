
#include "os/compat.h"

#include "os/debug.h"

#include "res/urr.h"
#include "misc/lua.h"
#include <stack>

namespace reaper {
namespace res {
namespace urr {

namespace { debug::DebugOutput derr("urr"); }

Navigate::~Navigate()
{ }

Urr::~Urr()
{ }

using namespace misc::lua;

struct Frame {
	int idx;
	int sz;
	bool at_end;
	Frame(int i) : idx(i), sz(0), at_end(false) { }
};

class LuaLurker : public Reader, public Writer
{
	Lua& lua;
	std::stack<Frame> ts;
public:
	LuaLurker(Lua& l, int);
	~LuaLurker();
	bool next();
	bool descend();
	bool descend(int);
	bool descend(std::string);
	bool up();


	bool read(bool&d);
	bool read(int&d);
	bool read(float&d);
	bool read(std::string&);
	int read_binary(unsigned char* buf, int n);

	bool write(std::string);
	bool write_binary(unsigned char* buf, int n);
};

LuaLurker::LuaLurker(Lua& l, int t)
 : lua(l)
{
	ts.push(t);
	if (lua.type(t) == type::Table) {
		lua.push();
		ts.top().at_end = !(lua.next(t) != 0);
	} else {
		ts.top().at_end = true;
	}
}

LuaLurker::~LuaLurker()
{

}


bool LuaLurker::next()
{
	if (ts.top().at_end)
		return false;
	lua.pop(1);
	ts.top().at_end = !(lua.next(ts.top().idx) != 0);
	if (ts.top().at_end)
		ts.top().sz = 1;
	return !ts.top().at_end;
}

bool LuaLurker::descend()
{
	if (ts.top().at_end)
		return false;
	ts.push(lua.top());
	if (lua.type(lua.top()) == type::Table) {
		lua.push_val(-1);
		lua.push();
		ts.top().at_end = !(lua.next(ts.top().idx) != 0);
		ts.top().sz = 3;
		return true;
	} else {
		lua.push();
		ts.top().at_end = true;
		ts.top().sz = 1;
		return true;
	}
}

bool LuaLurker::descend(int)
{
	return false;
}

bool LuaLurker::descend(std::string lbl)
{
	lua.push(lbl);
	lua.get_table(ts.top().idx);
	ts.push(lua.top());
	if (lua.type(lua.top()) == type::Table) {
		lua.push();
		ts.top().at_end = !(lua.next(ts.top().idx) != 0);
		ts.top().sz = 3;
	} else if (lua.type(lua.top()) == type::Nil) {
		lua.pop(1);
		ts.pop();
		return false;
	} else {
		lua.push();
		ts.top().at_end = true;
		ts.top().sz = 2;
		return true;
	}
	return !ts.top().at_end;
}

bool LuaLurker::up()
{
	lua.pop(ts.top().sz);
	ts.pop();
	return true;
}

bool LuaLurker::read(bool& b)
{
	b = lua.get(-2);
	return true;
}

bool LuaLurker::read(int& i)
{
	i = lua.get(-2);
	return true;
}

bool LuaLurker::read(float& f)
{
	f = lua.get(-2);
	return true;
}

bool LuaLurker::read(std::string& s)
{
	lua.push_val(-2);
	s = std::string(lua.get(-1));
	lua.pop(1);
	return true;
}

int LuaLurker::read_binary(unsigned char* buf, int n)
{
	return 0;
}

bool LuaLurker::write(std::string)
{
	return false;
}

bool LuaLurker::write_binary(unsigned char* buf, int n)
{
	return 0;
}


class LuaBrowser : public Urr
{
	Lua lua;
	int tbl;
public:
	LuaBrowser(const std::string&);
	~LuaBrowser();

	Reader* reader();
	Writer* writer();

};

LuaBrowser::LuaBrowser(const std::string& n)
{
	lua.grow_stack(200);
	Table t = load_table(lua, n);
	tbl = t.index();
}

LuaBrowser::~LuaBrowser()
{
}

Reader* LuaBrowser::reader()
{
	return new LuaLurker(lua, tbl);
}

Writer* LuaBrowser::writer()
{
	return 0;
//	return new LuaLurker(lua, tbl);
}

Urr* mk(const std::string& n)
{
	return new LuaBrowser(n);
}

}
}
}

