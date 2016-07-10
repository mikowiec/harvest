
#include "os/compat.h"

#include "os/debug.h"
#include "game/object_mgr.h"
#include "object/factory.h"
#include "object/object.h"
#include "object/phys.h"
#include "object/event.h"
#include "misc/parse.h"
#include "res/res.h"
#include "main/types_io.h"
#include "object/objectgroup.h"

#include "res/resource.h"
#include "phys/engine.h"
#include "world/world.h"

#include <iostream>


using namespace std;
using namespace reaper::misc;

namespace reaper {
namespace {
	reaper::debug::DebugOutput dout("scenario", 5);
}
namespace game {
namespace scenario {

using namespace object;

using res::resource;

ObjectMgr::ObjectMgr()
 : factory(factory::inst())
{
	init_loader("scenario");

}


void ObjectMgr::load_group(const std::string group)
{
	dout << "loading group " << group << '\n';
	res::resource<object::ObjectGroup>(group);
	for(int j = 0; j < Number_Companies; j++) {
		objs[std::make_pair(group, CompanyID(j))] = 0;
	}

}

class OnDead
{
	ObjectCounter& count;
	std::string group;
	object::ID id;
	CompanyID cid;
public:
	OnDead(ObjectCounter& c, std::string g, object::ID i, CompanyID ci)
	 : count(c), group(g), id(i), cid(ci)
	{ }
	void operator()()
	{
		--count[std::make_pair(group, cid)];
//		world::World::get_ref()->erase(id);
//		reaper::phys::PhysRef()->remove(id);
	}
};


class ObjAdder
{
	ObjectCounter& count;
	std::string group;
	reaper::phys::PhysRef pr;
public:
	ObjAdder(ObjectCounter& oc, std::string g, reaper::phys::PhysRef p)
	 : count(oc), group(g), pr(p)
	{ }
	void operator()(MkInfo mk)
	{
		SillyBase* ptr = object::factory::inst().gen_make(mk);
		// initialize will register the object and take over ownership
		ptr->initialize();

		misc::Ping oc(misc::mk_cmd(OnDead(count, group, ptr->stat().id(), ptr->stat().company())));
		ptr->events().death.add_listener(oc);
		++count[std::make_pair(group, ptr->stat().company())];
	}
};


void ObjectMgr::add_group(const std::string g)
{
	dout << "adding group " << g << '\n';
	object::ObjectGroup& og = res::resource<object::ObjectGroup>(g);
	reaper::phys::PhysRef pr;
	for_each(og.objs.begin(), og.objs.end(), ObjAdder(objs, g, pr));
}

struct {
	template<class T>
	void operator()(T t) { /*t->think();*/ }
} think_t;

void ObjectMgr::think(void)
{
	for_each(wr->begin_st(), wr->end_st(), think_t);
	for_each(wr->begin_dyn(), wr->end_dyn(), think_t);
}


void ObjectMgr::shutdown()
{
}

int ObjectMgr::nr_in_group(const std::string& group, CompanyID cid)
{
	return objs[std::make_pair(group, cid)];
}

void ObjectMgr::get_objectnames(std::set<std::string>& s)
{
	ObjectCounter::iterator c, e = objs.end();
	for (c = objs.begin(); c != e; ++c) {
		ObjectGroup& og
			= res::resource<ObjectGroup>(c->first.first);
		MkInfoVec::iterator m, me = og.objs.end();
		for (m = og.objs.begin(); m != me; ++m) {
			s.insert(m->name);
		}
	}
}



}
}
}



