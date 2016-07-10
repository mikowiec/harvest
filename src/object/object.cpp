#include "os/compat.h"

#include "object/object.h"
#include "object/object_impl.h"
#include "object/projectile.h"

#include "main/types_io.h"

#include <set>
#include <typeinfo>
#include <iostream>

namespace reaper {
namespace object {

std::set<Base*> objs;

int& obj_count() {
	static int count = 0;
	return count;
}

void obj_count_incr(Base* p)
{
	++obj_count();
//	objs.insert(p);
}	

void obj_count_decr(Base* p)
{
	--obj_count();
//	objs.erase(p);
}

int get_obj_count()
{

	std::set<Base*>::iterator c, e = objs.end();
/*
	for (c = objs.begin(); c != e; ++c) {
		std::cout << "lost: " << (*c)->get_id() << '\n';
	}
*/
	return obj_count();
}


ShotBase* laser(const Matrix &m, CompanyID c, ID id)
{
	return new Laser(m, c, id);
}

ShotBase* light_missile(const Matrix &m, CompanyID c, ID id)
{
	return new Missile("light_missile", m, c, id);
}

ShotBase* heavy_missile(const Matrix &m, CompanyID c, ID id)
{
	return new Missile("heavy_missile", m, c, id);
}





}
}

