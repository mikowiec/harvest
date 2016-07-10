/* $Id: factory.cpp,v 1.33 2003/06/04 17:43:06 pstrand Exp $ */
  
#include "os/compat.h"

#include <string>
#include <map>

#include "os/debug.h"
#include "main/types.h"
#include "main/types_io.h"
#include "misc/parse.h"
#include "misc/sequence.h"
#include "misc/stlhelper.h"
#include "misc/smartptr.h"
#include "misc/free.h"
#include "object/base.h"
#include "object/factory.h"
#include "object/factory_loaders.h"
#include "object/phys_data.h"
#include "object/obj_factory.h"
#include "res/config.h"
#include "res/res.h"
#include "res/resource.h"

#include "gfx/io.h"

namespace reaper {
namespace object {

namespace {
	debug::DebugOutput derr("object::factory");
}

using namespace res;

ConfigEnv MkInfo::empty;

MkInfo::MkInfo()
 : name("invalid"), mtx(Matrix::id()), cid(Nature), id(-1)
{ }


MkInfo::MkInfo(const std::string& n, const Matrix& m, ID i, CompanyID ci,
	       const ConfigEnv& inf,
	       const ConfigEnv& c)
 : name(n), mtx(m), cid(ci), id(i),
   info(inf.empty() ? factory::inst().info(name) : inf), cdata(c)
{ }



namespace factory {

using std::string;
using std::istream;


using std::map;
using misc::apply_to;

const ConfigEnv& Factory::info(const string& id)
{
	return info(id, id);
}


Factory::Factory()
{
}

const ConfigEnv& Factory::info(const string& real_id, const string& id)
{
	const ConfigEnv& env = resource<ConfigEnv>("objects/" + id);
	return env;
}

ObjBase* Factory::gen_make(MkInfo mk)
{
	std::map<string, create_fn>::iterator i = obj_creators.find(mk.name);

	if (i == obj_creators.end()) {
		return gen_create(MkInfo(mk.name, mk.mtx, mk.id, mk.cid, info(mk.name), mk.cdata));
	} else {
		return (i->second)(MkInfo(mk.name, mk.mtx, mk.id, mk.cid, info(mk.name), mk.cdata));
	}
}

ObjBase* Factory::gen_load(istream& is)
{
	ConfigEnv env(is, true);
	std::string name = env["object"];
	ID id = withdefault(env, "id", -1);
	Matrix m = read<Matrix>(env["matrix"]);
	if (! name.empty() && env.defined("matrix") && env.defined("company")) {
		std::map<string, create_fn>::iterator i = obj_creators.find(env["object"]);

		MkInfo mk(name, m, id, read<CompanyID>(env["company"]), info(name), env);
		if (i == obj_creators.end()) {
			ObjBase* o = gen_create(mk);
			return o;
		} else {
			ObjBase* o = (i->second)(mk);
			return o;
		}
	} else
		return 0;
}


Factory::~Factory()
{
//	misc::for_each(misc::seq(gen_info), misc::delete_it);
}

Factory& inst()
{
	static Factory fac;
	return fac;
}



}
}
}

