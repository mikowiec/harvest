
/* $Id: ptrstats.cpp,v 1.3 2002/12/29 17:04:39 pstrand Exp $ */


#include "os/compat.h"

#include <map>
#include <typeinfo>

#include "misc/sequence.h"

namespace reaper {

namespace misc {

typedef std::pair<const char*, int*> Stat;
typedef std::map<void*, Stat> StatMap;

StatMap& stats() {
	static StatMap sm;
	return sm;
}

void stats_add(const char* name, int* count, void* inst)
{
//	printf("us:add %s %d %x\n", name, *count, inst);
//	stats()[inst] = Stat(name, count);
}

void stats_delete(void* inst)
{
//	printf("us:del %s %x\n", stats()[inst].first, *stats()[inst].second, inst);
//	stats().erase(inst);
}

typedef std::pair<const void*, Stat> StatPair;

void stats_report()
{
//	printf(" ptrs:\n");
//	misc::Seq<StatMap::iterator> ss(seq(stats()));
//	for (;ss; ++ss) {
//		printf("%s %d %x\n", ss->second.first, *ss->second.second, ss->first);
//		debug::std_debug << *ss << '\n';
//	}
}

}
}

