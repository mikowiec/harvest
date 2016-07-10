
/* $Id: object_impl.h,v 1.26 2003/06/11 15:16:47 pstrand Exp $ */

#ifndef REAPER_OBJECT_OBJECT_IMPL_H
#define REAPER_OBJECT_OBJECT_IMPL_H

#include "object/base.h"
#include "object/base_data.h"
#include "object/controls.h"
#include "object/hull.h"
#include "object/renderer.h"
#include "object/ai.h"
#include "object/player_ai.h"
#include "object/phys.h"
#include "object/current_data.h"
#include "object/factory.h"
#include "object/reg.h"
#include "object/accessor.h"

#include "main/types_param.h"

#include <vector>

namespace reaper {
namespace object {

class StatAcc : public GetStat
{
	const SillyData& data;
public:
	StatAcc(const SillyData& d) : data(d) { }
	ID id() const { return data.get_id(); }
	CompanyID company() const { return data.get_company(); }
	Matrix mtx() const { return data.get_mtx(); }
	Point pos() const { return data.get_pos(); }
	float radius() const { return data.get_radius(); }
};

template<class Base>
class ObjImpl : public Base
{
protected:
	SillyData data;
	std::auto_ptr<StatAcc> stat_acc;
public:
	ObjImpl(const Matrix& mtx, CompanyID c, const std::string& n, ID i = -1)
	 : data(mtx, c, n, i),
	   stat_acc(new StatAcc(data.get_mtx(), data.get_radius()))
	{ }
	ObjImpl(const MkInfo& mk)
	 : data(mk.mtx, mk.cid, mk.name, mk.id),
	   stat_acc(new StatAcc(data))
	{ }

	event::Events& events() { return *data.events; }

	const GetStat& stat() const { return *stat_acc; };
};

class SillyImpl
 : public ObjImpl<SillyBase>,
   public Collide
{
protected:
	gfx::RenderInfo ri;
	hull::Standard hull;
public:
	SillyImpl(const MkInfo& mk);
	const gfx::RenderInfo* render(bool effects) const { return &ri; }
	void collide(CollisionInfo);
	void initialize();
};



}
}
#endif

