
#ifndef REAPER_MISC_LUA_H
#define REAPER_MISC_LUA_H

#include "main/exceptions.h"

#include <string>
#include <memory>
#include <iosfwd>

namespace reaper {
namespace misc {
namespace lua {




/*
 *
 * Simple, straightforward binding to the lua library.
 * If we're going to use lua extensivly, we should 
 * probably use something like luabind[1] or similar..
 * Just playing around at the moment.. ;)
 *  - peter
 *
 * ps: And the interface is a bit "too simple",
 *     ie, some operations abstracts too much,
 *     you still have to keep track of the stack
 *     even if it isn't visible...
 *
 * [1] http://luabind.sf.net
 *
 *
 *
 * I'm working on a generic interface to external
 * data sources, such as lua-scripts, so the littering
 * of #include "misc/lua.h" all over the place is temporary.
 *
 * 
 */






define_reaper_exception(lua_type_error);

class Val;
class Lua;

namespace type {
	enum T { None, Nil, Number, Boolean, String, Table, Function,
		 Userdata, Thread, LightUserdata };
}
typedef type::T Type;

/// Simple table abstraction, hides some of the cruft, but not all.
class Table
{
	Lua& lua;
	int idx;
public:
	Table(Lua& l, int i);

	// NOTE! indexed from zero, not one (as lua usually is)
	//       is this a good or bad idea, to change convention?
	Val operator[](int i);

	Val operator[](const std::string& k);

	template<class K>
	Val lookup(const K& k) {
		lua.push(k);
		lua.get_table(idx);
		return lua.get();
	}

	template<class K, class V>
	V lookup(const K& k, const V& v) {
		lua.push(k);
		lua.get_table(idx);
		Val val = lua.get();
		if (val.type() == type::Nil)
			return v;
		else
			return val;
	}

	template<class K, class V>
	void set(const K& k, const V& v) {
		lua.push(k);
		lua.push(v);
		set_table(idx);
	}

	int index() { return idx; }

	// We cannot make proper iterators (easily). 
	// Better then to do something simple, than something confusing..

	void begin_iter();
	Val get_key();
	Val get_val();
	bool more();
	void end_iter();

};

/// Value reader.
class Val
{
	Lua& lua;
	int idx;

	friend class Lua;
	Val(Lua& l, int index);
public:
	operator bool();
	operator int();
	operator float();
	operator double();
	operator std::string();
	operator Table();

	int index();

	Type type();
};



/** The Big Lua Wrapper.
 *  (some of the operations should be moved out to
 *   Val and Table and similar helpers..)
 */
class Lua
{
	friend class Val;
	struct Impl;
	std::auto_ptr<Impl> impl;

public:
	Lua();
	Lua(Impl*);
	~Lua();

	/// Get index to stack-top.
	int top() const;

	/// Add extra space to stack.
	int grow_stack(int extra);
	
	/// Change stack-pointer.
	void set(int index);

	/// Push the value pointed to by index.
	void push_val(int index);

	/// Pop elements from top of stack.
	void pop(int n);

	/// Remove element from stack.
	void remove(int index);

	/// Move top element to [index]
	void insert(int index);

	/// Replace element at [index] with top element
	void replace(int index);


	Type type(int index) const;
	// Returns true if the element is compatible with the given type:
	bool is_type(int index, Type type) const;

	std::string type_name(Type type) const;
	


	bool equal(int ix1, int ix2) const;
	bool rawequal(int ix1, int ix2) const;
	bool lessthan(int ix1, int ix2) const;

	Val get(int index = -1);

	// lua_CFunction to_cfunction(int index);

	void* to_userdata(int index) const;

	Lua* to_thread(int index) const;

	const void* to_pointer(int index) const;

	void push();
	void push(bool);
	void push(int);
	void push(double);
	void push(const char *);
	void push(const std::string&);
//	void push(lua_CFunction);
	void push_userdata(void*); // Push light user-data

	/// Concatentate N strings at the top of the stack.
	void concat(int n);


	int gccount(); // Memory in use (kbytes)
	int gcthreshold(); // When to gc
	void setgcthreshold(int new_threshold);

	void* new_userdata(size_t size);

	int get_metatable(int index);
	int set_metatable(int index);

	/** Load&compile a chunk of code, 
	 *  returns true if ok, throws bad_alloc if out of memory.
	 */
	bool load(const std::string& name, std::istream&);

	void call(int na, int nr);
	int pcall(int na, int nr, int e);

	void new_table();
	void get_table(int index); //< pop key, fetch from table at [index]
	void get_table_raw(int index); //< lua_rawget
	void set_table(int index); //<  pop key and val, set in table at [index]
	void set_table_raw(int index); //< lua_rawset

	/// pop key, push "next" (key,value) from table at [index]
	int next(int index);

	Val get_global(const std::string& var);


	void geti(int index, int n);
	void seti(int index, int n);
};

define_reaper_exception(lua_load_error);

/// Quick&simple version of above.
bool load_from_string(Lua& lua, const std::string& code);

	/** Load file and return the table with the same
	 *  name as the file.
	 */
Table load_table(Lua& lua, const std::string& name);

void print_table(Lua& lua, Table tbl);

template<typename T, typename I>
bool read_table(Lua& lua, Table obj, std::string label, I iter)
{
	Val val = obj.lookup(label);
	if (val.type() != type::Table)
		return false;
	Table tbl = val;
	tbl.begin_iter();
	typename T::value_type buf[T::N];
	while (tbl.more()) {
		Table vec = tbl.get_val();
		for (int i = 0; i < T::N; ++i) {
			buf[i] = vec[i];
		}
		*iter++ = T(buf);
		lua.pop(T::N);
	}
	tbl.end_iter();
	return true;
}

template<class V>
void read_vector(Lua& lua, Table tbl, V& vec, int n)
{
	for (int i = 0; i < n; ++i) {
		vec.get()[i] = tbl[i];
		lua.pop(1);
	}
}

template<class V, class D>
void read_vector_def(Lua& lua, Val tbl, V& vec, int n, D d)
{
	if (tbl.type() == type::Table)
		read_vector(lua, tbl, vec, n);
	else 
		vec = d;
}

template<class V>
void read_push_vector(Lua& lua, Table tbl, V& vec)
{
	int i = 0;
	do {
		Val val = tbl[i];
		if (lua.type(val.index()) == type::Nil) {
			lua.pop(1);
			return;
		}
		vec.push_back(val);
		lua.pop(1);
		++i;
	} while (true);
}

}
}
}

#endif

