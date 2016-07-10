#ifndef PHYS_PHYS_H
#define PHYS_PHYS_H

#include "main/types.h"
#include "object/base.h"

namespace reaper {
namespace phys {

const double min_dt = 0.005;
const float combat_cealing = 1000.0;     //Maximum height for you

using reaper::object::SillyPtr;
class Collision
{
public:
	Collision(SillyPtr o1, SillyPtr o2, Point p) : obj1(o1), obj2(o2), pos(p) {}

	SillyPtr obj1, obj2;
	Point pos;
};

}
}

#endif


/*
 * $Author: peter $
 * $Date: 2001/10/16 21:12:20 $
 * $Log: phys.h,v $
 * Revision 1.15  2001/10/16 21:12:20  peter
 * delete..
 *
 * Revision 1.14  2001/08/20 17:10:50  peter
 * obj.ptr
 *
 * Revision 1.13  2001/08/06 12:16:42  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.12.4.2  2001/08/03 13:44:14  peter
 * pragma once obsolete...
 *
 * Revision 1.12.4.1  2001/07/31 17:34:08  peter
 * testgren...
 *
 * Revision 1.12  2001/07/06 01:47:35  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 */
