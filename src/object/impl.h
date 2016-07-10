#ifndef REAPER_OBJECT_IMPL_H
#define REAPER_OBJECT_IMPL_H
#pragma once

#include "object/base.h"
#include <vector>

namespace reaper {
namespace object {
namespace impl {
/// @todo Move global time to correct place
double get_current_time();

/////////////////////////

class Silly : public SillyBase {
protected:
	Silly(const Matrix &m, float r, CompanyID c) :  
		SillyBase(m, r, c) 
	{}
};

class Static : public StaticBase {
protected:
	Static(const Matrix &m, float r, CompanyID c, double st = get_current_time()) :
		StaticBase(m ,r, c, st)
	{}
};

class Dynamic : public DynamicBase {
protected:
        Dynamic(const Matrix &m, float r, CompanyID c, double st = get_current_time()) : 
		DynamicBase(m, r, c, st)
	{}
};


class Player : public PlayerBase {
protected:
        Player(const Matrix &m, float r, CompanyID c, double st = get_current_time()) : 
		PlayerBase(m, r, c, st)
	{}
};

class Shot : public ShotBase
{
protected:
	Shot(const Matrix &m, float r, CompanyID c, const Vector &vel, double st, const float ls) :
		ShotBase(m, r, c, st, ls, vel)
	{}
public:
	~Shot() { }

};



}
}
}
#endif

/*
 * $Author: peter $
 * $Date: 2001/11/21 00:54:19 $
 * $Log: impl.h,v $
 * Revision 1.17  2001/11/21 00:54:19  peter
 * death...
 *
 * Revision 1.16  2001/07/30 23:43:28  macke
 * Häpp, då var det kört.
 *
 * Revision 1.15  2001/07/06 01:47:31  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.14  2001/06/09 01:58:57  macke
 * Grafikmotor reorg
 *
 * Revision 1.13  2001/05/10 22:05:43  picon
 * missile..
 *
 */

