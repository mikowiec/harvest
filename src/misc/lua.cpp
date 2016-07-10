
#include "os/compat.h"

#include "os/debug.h"

#include "main/types_io.h"
#include "misc/lua.h"

#include <sstream>
#include <iterator>
#include <iostream>

#include "res/res.h"
#include "res/resource.h"

#include "misc/parse.h"

extern "C" {
#include "lua50/lua.h"
#include "lua50/lualib.h"
}

namespace reaper {
namespace misc {
namespace lua {

namespace { debug::DebugOutput derr("lua"); }

struct Lua::Impl
{
	lua_State* state;
	Impl() : state(0) { }
	Impl(lua_State* s) : state(s) { }
};

Lua::Lua()
 : impl(new Impl())
{
	impl->state = lua_open();
	luaopen_base(impl->state);
	luaopen_string(impl->state);
	luaopen_table(impl->state);
	luaopen_math(impl->state);
	luaopen_io(impl->state);
	luaopen_debug(impl->state);
}

Lua::Lua(Impl* i)
 : impl(i)
{
}


Lua::~Lua()
{
	lua_close(impl->state);
}

int Lua::top() const
{
	return lua_gettop(impl->state);
}


int Lua::grow_stack(int extra)
{
	return lua_checkstack(impl->state, extra);
}

void Lua::set(int index)
{
	lua_settop(impl->state, index);
}

void Lua::push_val(int index)
{
	lua_pushvalue(impl->state, index);
}

void Lua::pop(int n)
{
	lua_pop(impl->state, n);
}

void Lua::remove(int index)
{
	lua_remove(impl->state, index);
}

void Lua::insert(int index)
{
	lua_insert(impl->state, index);
}

void Lua::replace(int index)
{
	lua_replace(impl->state, index);
}

Type Lua::type(int index) const
{
	switch (lua_type(impl->state, index)) {
	case LUA_TNONE:          return type::None;
	case LUA_TNIL:           return type::Nil;
	case LUA_TNUMBER:        return type::Number;
	case LUA_TBOOLEAN:       return type::Boolean;
	case LUA_TSTRING:        return type::String;
	case LUA_TTABLE:         return type::Table;
	case LUA_TFUNCTION:      return type::Function;
	case LUA_TUSERDATA:      return type::Userdata;
	case LUA_TTHREAD:        return type::Thread;
	case LUA_TLIGHTUSERDATA: return type::LightUserdata;
	}
	return type::None;
}

bool Lua::is_type(int index, Type type) const
{
	switch (type) {
	case type::Nil           : return lua_isnil(impl->state, index);
	case type::Number        : return lua_isnumber(impl->state, index);
	case type::Boolean       : return lua_isboolean(impl->state, index);
	case type::String        : return lua_isstring(impl->state, index);
	case type::Table         : return lua_istable(impl->state, index);
	case type::Function      : return lua_isfunction(impl->state, index);
	case type::Userdata      : return lua_isuserdata(impl->state, index);
	case type::Thread        : return lua_islightuserdata(impl->state, index);
	default: return false;
	}
	return false;
}

int type_to_luatype(Type t)
{
	switch (t) {
	case type::Nil           : return LUA_TNIL;
	case type::Number        : return LUA_TNUMBER;
	case type::Boolean       : return LUA_TBOOLEAN;
	case type::String        : return LUA_TSTRING;
	case type::Table         : return LUA_TTABLE;
	case type::Function      : return LUA_TFUNCTION;
	case type::Userdata      : return LUA_TUSERDATA;
	case type::Thread        : return LUA_TTHREAD;
	default: return LUA_TNONE;
	}
	return LUA_TNONE;
}

std::string Lua::type_name(Type type) const
{
	return lua_typename(impl->state, type_to_luatype(type));
}

bool Lua::equal(int ix1, int ix2) const
{
	return lua_equal(impl->state, ix1, ix2);
}

bool Lua::rawequal(int ix1, int ix2) const
{
	return lua_rawequal(impl->state, ix1, ix2);
}

bool Lua::lessthan(int ix1, int ix2) const
{
	return lua_lessthan(impl->state, ix1, ix2);
}

void* Lua::to_userdata(int index) const
{
	return lua_touserdata(impl->state, index);
}

Lua* Lua::to_thread(int index) const
{
	return new Lua(new Impl(lua_tothread(impl->state, index)));
}

const void* Lua::to_pointer(int index) const
{
	return lua_topointer(impl->state, index);
}

void Lua::push()
{
	lua_pushnil(impl->state);
}

void Lua::push(bool b)
{
	lua_pushboolean(impl->state, b);
}

void Lua::push(int n)
{
	lua_pushnumber(impl->state, n);
}

void Lua::push(double d)
{
	lua_pushnumber(impl->state, d);
}

void Lua::push(const char* s)
{
	lua_pushstring(impl->state, s);
}

void Lua::push(const std::string& s)
{
	lua_pushlstring(impl->state, s.c_str(), s.size());
}

/*
void Lua::push_func(lua_CFunction)
{

}
*/

void Lua::push_userdata(void* u)
{
	lua_pushlightuserdata(impl->state, u);
}

void Lua::concat(int n)
{
	lua_concat(impl->state, n);
}

int Lua::gccount()
{
	return lua_getgccount(impl->state);
}

int Lua::gcthreshold()
{
	return lua_getgcthreshold(impl->state);
}

void Lua::setgcthreshold(int new_threshold)
{
	lua_setgcthreshold(impl->state, new_threshold);
}

void* Lua::new_userdata(size_t size)
{
	return lua_newuserdata(impl->state, size);
}

int Lua::get_metatable(int index)
{
	return lua_getmetatable(impl->state, index);
}

int Lua::set_metatable(int index)
{
	return lua_setmetatable(impl->state, index);
}


typedef std::basic_streambuf<char> stream_buf;

typedef struct read_state {
	stream_buf* sbuf;
	char buf[10000];
};

const char* lua_stream_reader(lua_State*, void* data, size_t* size)
{
	read_state* st = static_cast<read_state*>(data);
	*size = st->sbuf->sgetn(st->buf, 10000);
	return (*size > 0) ? st->buf : 0;
}

bool Lua::load(const std::string& name, std::istream& is)
{
	read_state st;
	// the cast is just for gcc-2.95
	st.sbuf = static_cast<stream_buf*>(is.rdbuf());

	int r = lua_load(impl->state, lua_stream_reader,
			 static_cast<void*>(&st), name.c_str());
	switch (r) {
	case 0:             return true;
	case LUA_ERRSYNTAX: return false;
	case LUA_ERRMEM:    throw std::bad_alloc();
	}
	return false;
}

bool load_from_string(Lua& lua, const std::string& code)
{
	std::stringstream ss(code);
	return lua.load("<inline>", ss);
}

void merge(Lua& lua, Table tbl, Table ovr)
{
	ovr.begin_iter();
	while (ovr.more()) {
		Val k = ovr.get_key();
		Val v = ovr.get_val();
		lua.push_val(k.index());
		lua.push_val(v.index());
		lua.set_table(tbl.index());
	}
	ovr.end_iter();
}


Table load_table(Lua& lua, const std::string& name)
{
	std::string file_name = basename_from_path(name);
	std::string cname = "cache/"+name;
	bool loaded = false;
	
	if (res::is_newer(res::File, cname, name)) {
		res::res_stream rs(res::File, cname, res::throw_on_error);
		if (lua.load(name, rs))
			loaded = true;
		else
			derr << "error loading " << cname << "\n";
	}
	if (!loaded) {
		res::res_stream rs(res::File, name, res::throw_on_error);
		if (lua.load(name, rs))
			loaded = true;
	}
	if (!loaded)
		throw lua_load_error("error loading "+name);
	if (lua.pcall(0, -1, 0) != 0)
		throw lua_load_error("error initializing "+name);
	Table tbl = lua.get_global(file_name);
	Val ovr = lua.get_global("override");
	if (ovr.type() == type::Table)
		merge(lua, tbl, Table(ovr));
	return tbl;
}

void Lua::call(int na, int nr)
{
	lua_call(impl->state, na, nr);
}

int Lua::pcall(int na, int nr, int e)
{
	return lua_pcall(impl->state, na, nr, e);
}

void Lua::new_table()
{
	lua_newtable(impl->state);
}

void Lua::get_table(int index)
{
	return lua_gettable(impl->state, index);
}

void Lua::get_table_raw(int index)
{
	return lua_rawget(impl->state, index);
}

void Lua::set_table(int index)
{
	lua_settable(impl->state, index);
}

void Lua::set_table_raw(int index)
{
	lua_rawset(impl->state, index);
}


int Lua::next(int index)
{
	return lua_next(impl->state, index);
}

Val Lua::get_global(const std::string& var)
{
	push(var);
	get_table(LUA_GLOBALSINDEX);
	return get();
}

void Lua::geti(int index, int n)
{
	lua_rawgeti(impl->state, index, n);
}

void Lua::seti(int index, int n)
{
	lua_rawseti(impl->state, index, n);
}

Val Lua::get(int idx)
{
	return Val(*this, idx);

}


Val::Val(Lua& l, int i) : lua(l)
{
	idx = (i > 0) ? i : lua.top() + i + 1;
}

Val::operator bool()
{
	return lua_toboolean(lua.impl->state, idx);
}

Val::operator float()
{
	return lua_tonumber(lua.impl->state, idx);
}

Val::operator double()
{
	return lua_tonumber(lua.impl->state, idx);
}

Val::operator int()
{
	return static_cast<int>(lua_tonumber(lua.impl->state, idx));
}

Val::operator std::string()
{ 
	return std::string(lua_tostring(lua.impl->state, idx),
			   lua_strlen(lua.impl->state, idx));
}

Val::operator Table()
{
	return Table(lua, idx);
}

int Val::index() { return idx; }

Type Val::type() { return lua.type(idx); }

Table::Table(Lua& l, int i)
 : lua(l), idx(i)
{
	if (lua.type(idx) == type::String) {
		Table tbl = load_table(lua, lua.get(idx));
		idx = tbl.index();
	}
	if (lua.type(idx) != type::Table) {
		throw lua_type_error("not a table at " + write<int>(idx));
	}
}

Val Table::operator[](int i)
{
	lua.geti(idx, i+1);
	return lua.get();
}

Val Table::operator[](const std::string& k)
{
	return lookup(k);
}

void Table::begin_iter() { lua.push(); lua.push(); }
Val  Table::get_key()    { return lua.get(-2); }
Val  Table::get_val()    { return lua.get(-1); }
bool Table::more()       { lua.pop(1); return lua.next(idx) != 0; }
void Table::end_iter()   { }

void print_table(Lua& lua, Table tbl)
{
	derr << tbl.index() << " " << lua.type_name(lua.type(tbl.index())) << "\n";
	tbl.begin_iter();
	while (tbl.more()) {
		lua.push_val(-2);
		derr << std::string(lua.get(-1)) << " | " 
		     << lua.type_name(lua.type(-2)) << " "
		     << std::string(lua.get(-2)) << "\n";
		lua.pop(1);
	}
	tbl.end_iter();
}

}
}
}


