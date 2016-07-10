#ifndef REAPER_MISC_SINGLETON
#define REAPER_MISC_SINGLETON


#include "main/exceptions.h"
#include "misc/smartptr.h"

#include <string>
#include <typeinfo>

namespace reaper {
namespace misc {
namespace singleton {

define_reaper_exception(invalid);

template<typename T>
void throw_invalid() {
	throw invalid(std::string("Accessor<")+typeid(T).name()+"> invalid");
}

/*
 * All the messing around with base classes and casting is just
 * to enable a single repository of singleton pointers, used
 * for debugging and statistics (in misc/singleton.cpp)..
 */



class Base
{
public:
	virtual ~Base();
};

template<class T>
class Wrap : public Base, public T
{
public:
	Wrap() { }
	template<class A>
	Wrap(A a) : T(a) { }
	template<class A1, class A2>
	Wrap(A1 a1, A2 a2) : T(a1, a2) { }
};

struct inst_ptr
{
	Base* ptr;
	bool valid; /* "valid" is used to track illegal usage 
		       between shutdown and restart */

	inst_ptr() : ptr(0), valid(false) { }
};

typedef std::string type_id;

class Mgr
{
public:
	virtual ~Mgr();
	virtual void prepare(const type_id& id) = 0;
	virtual void shutdown(const type_id& id) = 0;
	virtual inst_ptr* lookup(const type_id& id) = 0;
};

Mgr& mgr();


template<class T>
class Impl
{
public:
	static void prepare()
	{
		mgr().prepare(typeid(T).name());
	}
	static void shutdown()
	{
		mgr().shutdown(typeid(T).name());
	}
};


template<class T>
class Accessor
{
	inst_ptr* ptr;
	T* get_inst_ptr()
	{
		if (!ptr->valid)
			throw_invalid<T>();
		if (ptr->ptr == 0) {
			ptr->ptr = new T();
		}
		return static_cast<T*>(ptr->ptr);
	}
public:
	Accessor() : ptr(mgr().lookup(typeid(T).name()))
	{ }
	T& inst()
	{
		return *get_inst_ptr();
	}
	T* operator->() { return get_inst_ptr(); }
};


}
}
}


#endif

