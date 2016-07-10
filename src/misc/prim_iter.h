
#ifndef REAPER_MISC_PRIM_ITER_H
#define REAPER_MISC_PRIM_ITER_H

#include <memory>

#include "misc/smartptr.h"

namespace reaper {
namespace misc {


template<class T>
class prim_iter_if
{
public:
	virtual ~prim_iter_if() { }
	virtual const T& get() const = 0;
	virtual void inc() = 0;
	virtual bool more() const = 0;
};



template<class T>
class prim_iter
{
	std::auto_ptr< prim_iter_if<T> > iter;
	prim_iter(const prim_iter&);
public:
	typedef T value_type;

	prim_iter(prim_iter_if<T>* i)
	 : iter(i)
	{ }
	const T& operator*() const {
		return iter->get();
	}
	prim_iter& operator++() {
		iter->inc();
		return *this;
	}
	bool more() const {
		return iter->more();
	}
};


template<class T, class C>
void fill_from_iter(prim_iter_if<T>* i, C& c)
{
	prim_iter<T> it(i);
	while (it.more()) {
		c.insert(*it);
		++it;
	}
}

}
}


#endif

