
#include "os/compat.h"


#include "main/enums.h"
#include "main/types_io.h"
#include "object/mkinfo.h"
#include "misc/parse.h"

#include "object/factory.h"

namespace reaper {

namespace object {

std::ostream& operator<<(std::ostream& os, const MkInfo& mk)
{
	misc::dumper(os)
	 ("object", mk.name)
	 ("matrix", mk.mtx)
	 ("id", mk.id)
	 ("company", mk.cid);
	res::ConfigEnv::iterator c, e = mk.cdata.end();
	for (c = mk.cdata.begin(); c != e; ++c) {
		if (c->first == "object" ||
		    c->first == "matrix" ||
		    c->first == "company" ||
		    c->first == "id")
			continue;
		os << c->first << ": " << c->second << '\n';
	}
	return os;
}


std::istream& operator>>(std::istream& is, MkInfo& mk)
{
	do {
		res::ConfigEnv obj(is, true);
		mk.name = obj["object"].str();
		if (mk.name.empty())
			continue;
		mk.mtx  = obj["matrix"];
		mk.cid  = read<CompanyID>(res::withdefault(obj, "company", "Nature"));
		mk.id   = res::withdefault(obj, "id", -1);
		mk.info = factory::inst().info(mk.name);
		mk.cdata = obj;
	} while (false);
	return is;
}


}
}

