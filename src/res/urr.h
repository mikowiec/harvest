
#ifndef REAPER_RES_URR_H
#define REAPER_RES_URR_H

/* Universal Resource Representation */

/*
 * Used as an internal, possibly intermediate, representation
 * for all resource data. 
 *
 * Used as an untyped backend for resources, with the typed 
 * frontend in "res/resource.h".
 *
 * The backend for this backend may be text files, binary data,
 * lua-scripts or just about anything...
 * 
 * It's up to the backend to interpret the external data
 * and decide how to present it in a tree-shaped form.
 *
 */

#include "misc/base.h"
#include "main/exceptions.h"
#include <string>

namespace reaper {
namespace res {
namespace urr {

define_reaper_exception(urr_type_error);


class Navigate
{
public:
	virtual bool next() = 0;
	virtual bool descend() = 0;
	virtual bool descend(int) = 0;
	virtual bool descend(std::string) = 0;
	virtual bool up() = 0;

	virtual ~Navigate();
};


class GRead
{
public:
	virtual bool read(bool&) = 0;
	virtual bool read(int&) = 0;
	virtual bool read(float&) = 0;
	virtual bool read(std::string&) = 0;

	virtual int read_binary(unsigned char* buf, int n) = 0;
};

class GWrite
{
public:
	virtual bool write(std::string) = 0;

	virtual bool write_binary(unsigned char* buf, int n) = 0;
};


class Reader : public Navigate, public GRead { };
class Writer : public Navigate, public GWrite { };
class Updater : public Reader, public Writer { };

class Urr
{

public:
	/* May return null if the action isn't supported */
	virtual Reader* reader() = 0;
	virtual Writer* writer() = 0;

	virtual ~Urr();
};


Urr* mk(const std::string&);


template<class T, class R>
T get(R& r, const std::string& lbl, T def = T())
{
	if (! r->descend(lbl))
		return def;
	T s;
	r->read(s);
	r->up();
	return s;
}

template<class V, class R>
bool get_vec(R& r, const std::string& lbl, V& v, int n, V def = V())
{
	if (! r->descend(lbl)) {
		v = def;
		return false;
	}
	for (int i = 0; i < n; ++i) {
		r->descend();
		r->read(v.get()[i]);
		r->up();
		r->next();
	}
	r->up();
	return true;
}

template<class V, class R>
bool get_push_vec(R& r, const std::string& lbl, V& v)
{
	if (! r->descend(lbl)) {
		return false;
	}
	if (r->descend()) {
		typename V::value_type e;
		do {
			r->read(e);
			v.push_back(e);
		} while (r->next());
		r->up();
	}
	r->up();
	return true;
}

template<class T, class R, class I>
bool get_arr_of_arr(R& r, const std::string& lbl, I iter)
{
	if (!r->descend(lbl))
		return false;
	typename T::value_type buf[T::N];
	/* this might look horribly inefficient, but it's not a bottleneck at
	 * the moment, if it becomes one, add a few higher-level ops to Reader.
	 */
	do {
		if (!r->descend())
			break;
		for (int i = 0; i < T::N; ++i) {
			r->descend();
			r->read(buf[i]);
			r->up();
			r->next();
		}
		*iter++ = T(buf);
		r->up();
	} while (r->next());
	r->up();

}

}
}
}



#endif

