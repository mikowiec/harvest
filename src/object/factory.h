
/* $Id: factory.h,v 1.21 2002/02/18 11:49:08 peter Exp $ */

#ifndef REAPER_OBJECT_FACTORY_H
#define REAPER_OBJECT_FACTORY_H

#include <iosfwd>
#include <map>
#include <string>

#include "main/types.h"
#include "misc/smartptr.h"
#include "res/config.h"
#include "object/mkinfo.h"

namespace reaper {

namespace res {
	class ConfigEnv;
}

namespace object {

class WeaponInfo;
class ShipInfo;
class GroundShipInfo;
class TurretInfo;
class SillyBase;
class ShotInfo;

/*
template<class T>
const T& memoize(const std::string& id, const res::ConfigEnv& env)
{
	typedef std::map<std::string, T> Memory;
	static Memory mem;

	Memory::iterator i = mem.find(id);
	if (i == mem.end())
		i = mem.insert(mem.begin(), std::make_pair(id, T(env)));
	return i->second;
}
*/

namespace factory {

typedef SillyBase ObjBase;


ObjBase* int_default_load(const std::string&, std::istream&);


class Factory {

	friend Factory& inst();
	Factory();
	std::map<std::string, ShotInfo*> shotinfo;

	typedef ObjBase* (*create_fn)(MkInfo);
	std::map<std::string, create_fn> obj_creators;

//	std::map<std::string, res::ConfigEnv*> gen_info;

	ObjBase* gen_load(std::istream&);

public:
	~Factory();

	const res::ConfigEnv& info(const std::string& id);
	const res::ConfigEnv& info(const std::string& real_id, const std::string& id);

	/** Load an object from an istream. Make vc happy...*/
	template<class T>
	misc::SmartPtr<T> load(std::istream& is, TMap<T>) {
		return dynamic_cast<T*>(gen_load(is));
	}

	/** Create an object given a name and initial state. */
	template<class T>
	misc::SmartPtr<T> create(MkInfo mk, TMap<T>) {
		ObjBase* o = gen_make(mk);
		return dynamic_cast<T*>(o);
	}
	ObjBase* gen_make(MkInfo);

	/** Register an object creator. */
	void register_object(const std::string& id, create_fn fn)
	{
		obj_creators[id] = fn;
	}
};

Factory& inst();



}
}
}

#endif

