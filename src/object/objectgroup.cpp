
#include "os/compat.h"

#include <vector>

#include "object/objectgroup.h"
#include "misc/stlhelper.h"
#include "misc/sequence.h"
#include "res/resource.h"

namespace reaper {
namespace object {

using namespace res;

class ObjGroupConf : public res::NodeConfig<ObjectGroup>
{

	Ident dir;

public:
	ObjGroupConf(IdentRef d) : dir(d) { }
	typedef tp<ObjectGroup>::ptr Ptr;

	Ptr create(IdentRef id) {
		try {
			Ptr p = Ptr(new ObjectGroup());
			res_stream ol(File, dir + "/" + id, res::throw_on_error);
			while (ol) {
				object::MkInfo mk;
				ol >> mk;
				if (!mk.name.empty())
					p->objs.push_back(mk);
			}
			return p;
		} catch (resource_not_found) {
			return 0;
		}
	}
	void save(IdentRef id, const ObjectGroup& grp) {
		res_out_stream os(File, dir + "/" + id);
		misc::copy(misc::cseq(grp.objs), std::ostream_iterator<MkInfo>(os, "\n"));
	}
};

void init_loader(IdentRef d)
{
	res::define_node<ObjectGroup>(new ObjGroupConf(d));
}

}
}


