
#include "os/compat.h"

#include "os/debug.h"
#include "gfx/managers.h"

#include "object/event.h"
#include "object/base_data.h"

namespace reaper {
namespace object {

namespace { debug::DebugOutput derr("object::base"); }


SillyData::SillyData(const Matrix &mtx, CompanyID c,
		     const std::string& n, ID i)
 : id(i), name(n), dead(false), 
   radius(gfx::MeshMgr::get_ref()->get_radius(name)), 
   company(c), m(mtx), events(new event::Events())
{ }

SillyData::SillyData(const Matrix &mtx, CompanyID c,
		     const std::string& n, float r, ID i)
 : id(i), name(n), dead(false), 
   radius(r), company(c), m(mtx), events(new event::Events())
{ }

SillyData::~SillyData()
{
	delete events;
}

ID SillyData::get_id() const
{
	return id;
}

CompanyID SillyData::get_company() const
{
	return company;
}

bool SillyData::is_dead() const
{
	return dead;
}

Point SillyData::get_pos() const
{
	return m.pos();
}

float SillyData::get_radius() const
{
	return radius;
}

const Matrix& SillyData::get_mtx() const
{
	return m;
}

void SillyData::set_mtx(const Matrix& mtx)
{
	m = mtx;
}

const std::string& SillyData::get_name() const
{
	return name;
}

}
}



