#ifndef REAPER_MISC_UNIQUE_H
#define REAPER_MISC_UNIQUE_H

#include <string>

namespace reaper {
namespace misc {

class Unique
{
	int val;
public:
	static void reset();

	Unique(const Unique &x) : val(x.val) {}
	Unique(const std::string &);
	Unique(const char *);

	std::string get_text() const;

	bool operator< (const Unique &x) const { return val <  x.val; }
	bool operator==(const Unique &x) const { return val == x.val; }
	bool operator!=(const Unique &x) const { return val != x.val; }
};

}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2002/04/12 18:51:13 $
 * $Log: unique.h,v $
 * Revision 1.4  2002/04/12 18:51:13  pstrand
 * dynobj i quadtree
 *
 * Revision 1.3  2002/02/26 22:33:49  pstrand
 * mackefix
 *
 * Revision 1.2  2001/08/06 12:16:33  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.1.4.1  2001/08/03 13:44:08  peter
 * pragma once obsolete...
 *
 * Revision 1.1  2001/07/06 01:47:28  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 */
