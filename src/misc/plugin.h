
#ifndef REAPER_MISC_PLUGIN_H
#define REAPER_MISC_PLUGIN_H

#include <string>
#include <map>

#include "misc/base.h"

namespace reaper {
namespace os {
	namespace dynload { class DynLoader; }
}
namespace misc {

typedef void* (*VoidCreator1)(void*);

namespace preloaded {
void add_plugin_creator(std::string, VoidCreator1);
VoidCreator1 find_plugin(std::string);
}


struct Cache { };
struct NoCache { };





class PluginBase : misc::NoCopy
{
public:
	struct Creator {
		os::dynload::DynLoader* dyn;
		VoidCreator1 creator;
	};

	typedef std::map<std::string, Creator> Creators;
	typedef std::map<std::string, void*> Plugins;
private:
	Creators creators;
	Plugins plugins;
protected:
	PluginBase();
	~PluginBase();

//	VoidCreator1 load(const std::string& name);
	void* create(const std::string& name, void* arg, Cache);
	void* create(const std::string& name, void* arg, NoCache = NoCache());

public:
	void erase();
};


/// General Plugin-creator

template<typename T, class CachePolicy = NoCache>
class PluginCreator
 : public PluginBase
{
public:
	template<typename A>
	T* create(const std::string& name, A* a) {
		return static_cast<T*>(PluginBase::create(name, a, CachePolicy()));
	}
};


/** Helper for the normal simple case.
  * One (callback) interface, creates a new object each time.
  */

template<typename PluginType, typename Interface, class CachePolicy = NoCache>
class Plugin : misc::NoCopy
{
	typedef PluginCreator<PluginType, CachePolicy> Proxy;
	Proxy proxy;
//	typedef PluginType* (*CreateFunc)(Interface*);
public:
	Plugin() { } // : createfunc(0) { }

	PluginType* create(const std::string& name, Interface* i)
	{
		return proxy.create(name, i);
	}

	void erase()
	{
		proxy.erase();
	}
};


}
}

#endif



