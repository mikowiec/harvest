

#ifndef REAPER_GAME_OBJECT_MGR_H
#define REAPER_GAME_OBJECT_MGR_H

#include "main/enums.h"
#include "object/baseptr.h"
#include "world/worldref.h"
#include "phys/engine.h"

#include <set>
#include <map>

namespace reaper {
namespace object {
	namespace factory { class Factory; }
}
namespace game {
namespace scenario { 



typedef std::map<std::pair<std::string, CompanyID>, int> ObjectCounter;

class ObjectMgr
{
	ObjectCounter objs;

	object::factory::Factory& factory;
	world::WorldRef wr;
public:
	ObjectMgr();

	/// creates the objects for scenario "scen".
	/// (They are not added to the manager until
	/// add_objects() is called)
	void load_group(const std::string scen);

	/// add the objects to the manager and to the world 
	void add_group(const std::string group);

	/// perform ai for intelligent objects
	void think();

	int nr_in_group(const std::string& group, CompanyID id);

	void shutdown();

	void get_objectnames(std::set<std::string>&);
};

}
}
}
#endif

