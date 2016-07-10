
#include "os/compat.h"
#include "os/debug.h"
#include "game/mission.h"
#include "res/resource.h"
#include "res/config.h"
#include "misc/parse.h"
#include "misc/free.h"
#include "main/types_io.h"
#include "game/scenario_states.h"
#include "misc/sequence.h"
#include "world/world.h"

#include "object/event.h"
#include "misc/command.h"

namespace reaper {
namespace game {
namespace scenario {

namespace { debug::DebugOutput derr("mission", 5); }

using namespace std;
using namespace res;
using namespace misc;

typedef vector<string> strvec;

Goal::~Goal() { }

class GoalImpl : public Goal
{
	string nxt;
public:
	GoalImpl(const strvec& a) : nxt(a.back()) { }
	string next() const { return nxt; }
};

class TimeOut : public GoalImpl
{
	float timeout;
public:
	TimeOut(const strvec& a)
	 : GoalImpl(a), timeout(misc::stof(a[1]))
	{ }
	bool test(double time, ObjectMgr& o, ai::MsgQueue& m) {
//		derr << "timeout " << time << " > " << timeout << '\n';
		return time > timeout;
	}
};

class Goto : public GoalImpl
{
public:
	Goto(const strvec& a) : GoalImpl(a) { }
	bool test(double time, ObjectMgr& o, ai::MsgQueue& m) {
//		derr << "goto\n";
		return true;
	}
};

class Msg : public GoalImpl
{
	ai::MsgName msg_name;
	CompanyID cid;
public:
	Msg(const strvec& a) : GoalImpl(a), cid(misc::str2company(a[1]))
	{
		// FIXME
		msg_name = ai::MSG_DEST_REACHED;
	}
	bool test(double time, ObjectMgr& o, ai::MsgQueue& mq) {
//		derr << "msg\n";
		if (!mq.empty()) {
			ai::Message msg = mq.top();
			mq.pop();
			return msg.name == msg_name && msg.sender == cid;
		}
		return false;
	}
};

class Objects : public GoalImpl
{
	int n;
	string grp;
	CompanyID cid;
public:
	Objects(const strvec& a)
	 : GoalImpl(a),
	   n(misc::stol(a[1])),
	   grp(a[2]),
	   cid(misc::str2company(a[3]))
	{
		
	}
	bool test(double time, ObjectMgr& om, ai::MsgQueue& m) {
//		derr << "objects " << grp << " "
//		     << cid << " : " << om.nr_in_group(grp, cid)
//		     << " <= " <<  n << '\n';
		return om.nr_in_group(grp, cid) <= n;
	}
};

Goal* mk_goal(const string& s)
{
	strvec arg;
	misc::split(s, back_inserter(arg));
	string g = arg.front();
	if (g == "goto")
		return new Goto(arg);
	else if (g == "timeout")
		return new TimeOut(arg);
	else if (g == "objects")
		return new Objects(arg);
	else if (g == "msg")
		return new Msg(arg);

	derr << "Unknown goal type: " << g << '\n';
	return 0;
}

void read(const ConfigEnv& env, Scenario* scen, string pfx)
{
	if (pfx.empty() || pfx == "end")
		return;
	Mission* m = new Mission();
	m->name = pfx;
	m->dialog = env[pfx+"_dialog"].str();
	misc::split(env[pfx+"_objects"], back_inserter(m->objectgroups));
	int i = 1;
	do {
		string goal(pfx+"_goal"+misc::ltos(i++));
		if (!env.defined(goal))
			break;
		Goal* g = mk_goal(env[goal]);
		m->goals.push_back(g);
		if (scen->missions.find(g->next()) == scen->missions.end())
			read(env, scen, g->next());
	} while (true);
	scen->missions[pfx] = m;
}

class MissionCreate : public NodeConfig<Scenario>
{

public:
	typedef tp<Scenario>::ptr Ptr;

	Ptr create(IdentRef id)
	{
		const ConfigEnv& env = resource<ConfigEnv>("scenario/" + id);
		if (env.empty())
			return Ptr(0);
		Scenario* scen = new Scenario;
		read(env, scen, "start");
		return Ptr(scen);
	}

};

struct InitLoader {
	InitLoader() {
		res::define_node(new MissionCreate());
	}
	~InitLoader() { }
} stat_init;


MissionState::MissionState(Mission* mm, ObjectMgr& o, MsgQueue& q)
 : StateBase(o, q), m(mm), dead(false)
{
	for (size_t i = 0; i < m->objectgroups.size(); ++i)
		om.load_group(m->objectgroups[i]);
}

void MissionState::on_death()
{
	dead = true;
}

string MissionState::on_enter()
{
	derr << "Entering " << m->name << '\n';
	for (size_t i = 0; i < m->objectgroups.size(); ++i)
		om.add_group(m->objectgroups[i]);
	world::WorldRef()->get_local_player()->
		events().death.add_listener(misc::mk_cmd(this, &MissionState::on_death));
	return m->dialog;
}

string MissionState::update(float time)
{
	if (dead)
		return "end";
	for (size_t i = 0; i < m->goals.size(); ++i) {
		if (m->goals[i]->test(time, om, mq))
			return m->goals[i]->next();
	}
	return m->name;
}

void MissionState::on_exit()
{
	derr << "Exiting " << m->name << '\n';
}

Mission::Mission()
{
}

Mission::~Mission()
{
	for_each(seq(goals), delete_it);
}

Scenario::Scenario()
{
}

Scenario::~Scenario()
{
	for_each(seq(missions), delete_it);
}


}
}
}


