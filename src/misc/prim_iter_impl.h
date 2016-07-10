
#ifndef REAPER_MISC_PRIM_ITER_IMPL_H
#define REAPER_MISC_PRIM_ITER_IMPL_H

#include "misc/prim_iter.h"

namespace reaper {
namespace misc {


template<class T, class C>
class prim_iter_std_impl : public prim_iter_if<T>
{
	const C& container;
	typename C::const_iterator cur, end;
public:
	prim_iter_std_impl(const C& c)
	 : container(c), cur(c.begin()), end(c.end())
	{ }
	const T& get() const {
		return *cur;
	}
	void inc() {
		++cur;
	}
	bool more() const {
		return cur != end;
	}
};

template<class C>
prim_iter_if<typename C::value_type>* mk_iter(const C& c) {
	return new prim_iter_std_impl<typename C::value_type,C>(c);
}


template<class T, class C>
class prim_iter_owned_impl : public prim_iter_if<T>
{
	C* container;
	typename C::const_iterator cur, end;
	prim_iter_owned_impl(const prim_iter_owned_impl&);
public:
	prim_iter_owned_impl(C* c)
	 : container(c), cur(c->begin()), end(c->end())
	{ }
	~prim_iter_owned_impl()
	{
		delete container;
	}
	const T& get() const {
		return *cur;
	}
	void inc() {
		++cur;
	}
	bool more() const {
		return cur != end;
	}
};


template<class C>
prim_iter_if<typename C::value_type>* mk_owned_iter(C* c) {
	return new prim_iter_owned_impl<typename C::value_type,C>(c);
}



}
}


#endif


