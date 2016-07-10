#ifndef REAPER_MISC_CREATOR_H
#define REAPER_MISC_CREATOR_H

namespace reaper
{
namespace misc
{

template<class Base>
class CreateBase {
public:
	virtual Base* create() = 0;
};

template<class Base, class Derived>
class Creator : public CreateBase<Base> {
public:
	virtual Base* create() { return new Derived(); }
};

}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2002/09/29 12:35:53 $
 * $Log: creator.h,v $
 * Revision 1.4  2002/09/29 12:35:53  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.3  2001/08/06 12:16:30  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.2.4.1  2001/08/03 13:44:05  peter
 * pragma once obsolete...
 *
 * Revision 1.2  2001/07/06 01:47:25  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.1  2001/03/22 02:09:12  macke
 * no message
 *
 */

