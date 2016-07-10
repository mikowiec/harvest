#ifndef REAPER_MISC_MAYBE_H
#define REAPER_MISC_MAYBE_H

namespace reaper {
namespace misc {

// Maybe typ... användbar? tja.. återstår att se ;)

template<class T>
class Maybe {
	T elt;
	bool used;
public:
	Maybe() : used(false) { }
	Maybe(const T& t) : used(true) {
		elt = t;
	}
	Maybe& operator=(const T& t) {
		elt = t;
		used = true;
		return *this;
	}
	Maybe& operator=(bool b) {
		used = b;
		return *this;
	}
	bool is_just() const {
		return used;
	}
	T& get() { return elt; }
	operator T&() { return elt; }
	operator T() { return elt; }
	operator bool() {
		return used;
	}
	void use() { used = true; }
	void clear() { used = false; }
};

}
}

#endif


/*
 * $Author: peter $
 * $Date: 2001/08/06 12:16:31 $
 * $Log: maybe.h,v $
 * Revision 1.6  2001/08/06 12:16:31  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.5.4.1  2001/08/03 13:44:06  peter
 * pragma once obsolete...
 *
 * Revision 1.5  2001/07/06 01:47:26  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.4  2001/05/03 04:19:13  peter
 * *** empty log message ***
 *
 * Revision 1.3  2001/04/28 23:49:47  peter
 * *** empty log message ***
 *
 * Revision 1.2  2001/01/01 15:34:23  peter
 * no message
 *
 * Revision 1.1  2000/09/28 10:42:04  peter
 * *** empty log message ***
 *
 */

