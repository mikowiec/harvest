
#include "os/compat.h"

#include "os/debug.h"
#include "os/dynload.h"

#include "misc/sequence.h"
#include "misc/free.h"
#include "misc/plugin.h"
#include "world/world.h"

#include <stdio.h>

namespace reaper {
namespace misc {

	debug::DebugOutput derr("plugin", 5);

namespace preloaded {

struct Preloaded {
	std::map<std::string, VoidCreator1> pre;
};

Preloaded& preloaded_ref()
{
	static Preloaded p;
	return p;
}

void add_plugin_creator(std::string s, VoidCreator1 c)
{
	preloaded_ref().pre[s] = c;
}

VoidCreator1 find_plugin(std::string s)
{
	Preloaded& p = preloaded_ref();
	std::map<std::string, VoidCreator1>::iterator i = p.pre.find(s);
	return (i != p.pre.end()) ? i->second : 0;
}
}


using std::string;

int del_crts(const std::pair<std::string, PluginBase::Creator>& p) {
	delete p.second.dyn;
	return 0;
}

PluginBase::PluginBase()
{
}

PluginBase::~PluginBase()
{
	erase();
}

void* PluginBase::create(const string& name, void* arg, Cache)
{
	Plugins::iterator i(plugins.find(name));
	if (i == plugins.end())
		return (plugins[name] = create(name, arg));
	else
		return i->second;
}

void* PluginBase::create(const string& name, void* arg, NoCache)
{
	Creators::iterator i(creators.find(name));
	VoidCreator1 fn;
	if (i != creators.end()) {
		fn = i->second.creator;
	} else {
		VoidCreator1 v = preloaded::find_plugin(name);
		if (v == 0) {
			Creator crt;
			crt.dyn = new os::dynload::DynLoader(name);
			crt.creator = crt.dyn->load(string("create_") + name);
			creators[name] = crt;
			v = crt.creator;
			derr << "using dynamically loaded version of " + name + '\n';
		} else {
			derr << "using embedded version of " + name + '\n';
		}
		fn = v;
	}
	return fn(arg);
}

void PluginBase::erase()
{
	for_each(seq(creators), del_crts);
}

}
}



