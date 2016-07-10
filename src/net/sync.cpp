

#include "os/compat.h"

#include "os/debug.h"
#include "os/reltime.h"

#include "net/sync.h"

#include "main/types_io.h"
#include "misc/sequence.h"
#include "misc/free.h"
#include "misc/parse.h"
#include "misc/smartptr.h"

#include <deque>
#include <map>
#include <set>

namespace reaper {
namespace net {

namespace { debug::DebugOutput derr("net::sync"); }

State::~State()
{ }

SyncIF::~SyncIF()
{ }

using namespace std;
using namespace misc;

typedef int ID;

typedef deque<pair<Timestamp, misc::SmartPtr<State> > > StateQueue;
typedef map<ID, StateQueue> StatesMap;

typedef map<ID, SyncIF*> SyncObjs;
typedef SyncObjs::iterator ObjsIter;

struct Synchronizer_impl
{
	SyncObjs objs;
	StatesMap states;
	set<ID> local_ids;
};

Synchronizer::Synchronizer()
{
	impl = new Synchronizer_impl();
}

Synchronizer::~Synchronizer()
{
	delete impl;
}

void Synchronizer::add(ID id, SyncIF* sif)
{
	impl->objs[id] = sif;
}

void Synchronizer::spec_local_id(ID id)
{
	derr << "considering " << id << " as local\n";
	impl->local_ids.insert(id);
}

void Synchronizer::update(StateReps& reps)
{
	Timestamp now = os::time::get_rel_time();

	StateReps out;

	for (StateReps::iterator i = reps.begin(); i != reps.end(); ++i) {
		StateRep rep = read<StateRep>(*i);
		StateQueue& q = impl->states[rep.id];
		while (!q.empty() && q.front().first < rep.sync) {
//			derr << "discarding " << q.front().first << ' ' << rep.sync << '\n';
			q.pop_front();
		}
		if (q.empty()) {
			derr << "no state saved for: " << rep.id << " at time: " << std::hex << rep.sync << '\n';
		} if (q.front().first > rep.sync) {
//			derr << "old update: " << std::hex << rep.sync << '\n';
		} else {
//			derr << "merge: " << std::hex << q.front().first << ' ' << std::hex << rep.sync << '\n';
			q.front().second->merge(rep.rep);
			q.pop_front();
		}
	}

	for (ObjsIter i = impl->objs.begin(); i != impl->objs.end(); ++i) {
		StateQueue& q = impl->states[i->first];
		State* st = i->second->get_current_state();
		misc::SmartPtr<State> stp(st);
		q.push_back(make_pair(now, stp));
//		derr << "saving " << now << '\n';

		if (impl->local_ids.find(i->first) != impl->local_ids.end()) {
			StateRep sr;
			sr.id = i->first;
			sr.sync = now;
			sr.rep = stp->serialize();
			std::ostringstream os;
			os << sr;
			out.push_back(os.str());
		}
	}
	reps.swap(out);
	derr << "update, sending " << reps.size() << " reps\n";
}


}
}

