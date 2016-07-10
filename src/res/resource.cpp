
#include "os/compat.h"

#include "res/resource.h"

#include "misc/sequence.h"
#include "misc/stlhelper.h"
#include "misc/free.h"

namespace reaper {
namespace res {

using namespace misc;

misc::prim_iter_if< std::pair<const Ident, Managed*> >* Mgr::roots()
{
	return misc::mk_iter(nodes);
}

void Mgr::flush()
{
	map_snd(seq(nodes), std::mem_fun(&Managed::flush));
}

void Mgr::add(IdentRef id, Managed* n)
{
	nodes[id] = n;
}

void Mgr::del(Managed* n)
{
	std::map<std::string, Managed*>::iterator c, e = nodes.end();
	for (c = nodes.begin(); c != e; ++c) {
		if (c->second == n) {
			nodes.erase(c);
			return;
		}
	}
}

Mgr& mgr()
{
	static Mgr mgr_inst;
	return mgr_inst;
}

void Mgr::shutdown()
{
	for_each(seq(nodes), delete_it);
	nodes.clear();
}


Managed::Managed(IdentRef id)
{
	mgr().add(id, this);
}

Managed::~Managed()
{
}



typedef Managed* MPtr;

MPtr& ptr(const std::string& id)
{
	static std::map<std::string, Managed*> ptrmap;
	return ptrmap[id];
}

Managed* get_res_ptr(const std::string& id)
{
	return ptr(id);
}

void reinit_ptr(const std::string& id, Managed* p)
{
	delete ptr(id);
	ptr(id) = p;
}



}
}

