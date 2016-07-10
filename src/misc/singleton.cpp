

#include "os/compat.h"

#include "os/debug.h"

#include "os/debug.h"
#include "misc/sequence.h"

#include "misc/singleton.h"
#include "misc/sequence.h"

#include <stdio.h>
#include <map>

namespace reaper {

namespace misc {
namespace singleton {

namespace { debug::DebugOutput derr("singleton"); }

struct print_warning {
	template<class K, class T>
	void operator()(std::pair<K,T> p) const {
		derr << "warning, " << p.first << " not shutdown properly!\n";
	}
};

Base::~Base()
{ }

Mgr::~Mgr()
{ }

class MgrImpl : public Mgr
{
	std::map<type_id, inst_ptr> insts;
public:
	void prepare(const type_id& id)
	{
		insts[id].valid = true;
//		inst_ptr& p = insts[id];
//		printf("restart: %s: %x %x %d\n", id.c_str(), &p, p.ptr, p.valid);
	}

	void shutdown(const type_id& id)
	{
		inst_ptr& p = insts[id];
		p.valid = false;
		delete p.ptr;
		p.ptr = 0;
//		inst_ptr& p2 = insts[id];
//		printf("shutdown: %s: %x %x %d\n", id.c_str(), &p2, p2.ptr, p2.valid);
	}

	inst_ptr* lookup(const type_id& id) {
//		inst_ptr& p = insts[id];
//		printf("lookup: %s: %x %x %d\n", id.c_str(), &p, p.ptr, p.valid);
		return &insts[id];
	}

	virtual ~MgrImpl() {
//		misc::for_each(misc::seq(insts), print_warning());
	}
};

Mgr& mgr()
{
	static MgrImpl impl;
	return impl;
}


}
}
}


