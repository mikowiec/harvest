/*
 * Highlevel network management and object synchonization.
 *
 */

#include "os/compat.h"

#include <vector>
#include <string>
#include <iostream>
#include <queue>

#include "os/debug.h"
#include "msg/event.h"
#include "os/reltime.h"
#include "os/worker.h"
#include "os/concurrent.h"

#include "net/net.h"
#include "misc/parse.h"
#include "misc/free.h"
#include "misc/sequence.h"
#include "res/config.h"
#include "net/sync.h"

#include "net/game_mgr.h"
#include "net/netgame.h"
#include "net/net.h"

#include "net/server.h"

#include "object/phys.h"
#include "object/base_data.h"

#include "world/world.h"
#include "main/types.h"
#include "main/types_io.h"

namespace reaper {
namespace net {

namespace { debug::DebugOutput derr("game_mgr", 5); }

using namespace misc;
using std::map;
using std::string;
using std::deque;

// typedef map<object::ID, deque<ObjState> > SaveObjStates;
// typedef SaveObjStates::iterator save_state_iter;

struct GameMgr_impl
{
	NetGame net;
	os::event::EventSystem es;

	world::WorldRef wr;

	deque<object::ID> local_ids;

//	IntObjStates obj_states;
//	SaveObjStates saved_obj_states;

	os::time::RelTime last_sync, last_sync_s, last_sync_r, last_save;
	os::time::RelTime sync_interval;
	os::event::EventFilter* ef;
	string map;

	Sync sync;

	GameMgr_impl(os::gfx::Gfx& gx)
	 : es(gx), 
	   last_sync(0), last_sync_s(0), last_sync_r(0), last_save(0),
	   sync_interval(1000), ef(0)
	{
		es.disable();
	}

	~GameMgr_impl()
	{
	}

//	void diffify(const ObjState&);

//	void save_state();
//	void send_sync();
//	void recv_sync();


	void init_game(bool netw, std::string srv, bool split)
	{
		if (netw)
			ef = net.connect(srv);
		map = (split) ? "hw_event_split_map" : "hw_event_game_map";
	}
};


GameMgr::GameMgr(os::gfx::Gfx& gx)
{
	net::SyncImpl::prepare();
	impl = new GameMgr_impl(gx);
}

GameMgr::~GameMgr()
{
	if (impl)
		delete impl;
	net::SyncImpl::shutdown();
}


class ServerJob : public os::worker::Job {
	net::GameServer gs;
public:
	bool operator()() {
		gs.run();
		return false;
	}
};

void GameMgr::start_server()
{
	os::worker::worker()->spawn_job(new ServerJob());
}

/*
void GameMgr::add_sync(NetObjPtr o)
{
//	derr << "new obj: " << o.get() << '\n';
	impl->obj_states[o->get_id()] = o;
}
*/

void GameMgr::update()
{
	if (!impl->net.is_connected())
		return;

	os::time::RelTime now = os::time::get_rel_time();
	if (now < (impl->last_sync + impl->sync_interval))
		return;
	impl->last_sync = now;

	StateReps rep;
	impl->net.get_objstates(rep);
	impl->sync->update(rep);
	impl->net.put_objstates(rep);

}


void GameMgr::init(bool netw, std::string srv, bool split)
{
	impl->init_game(netw, srv, split);
}


PlayerID GameMgr::alloc_id(bool observer)
{
	PlayerID id = impl->net.join(observer);
	if (!observer) {
		impl->local_ids.push_back(id);
		impl->sync->spec_local_id(id);
	}
	return id;
}


const Players& GameMgr::players()
{
	return impl->net.players();
}

bool GameMgr::get_multistatus()
{
	return impl->net.get_multistatus();
}


bool GameMgr::start_req()
{
	if (impl->net.is_connected())
		return impl->net.start_net_game();
	else
		return true;
}

void GameMgr::end()
{
	impl->net.shutdown();
}


void GameMgr::go_go_go()
{
	impl->es.set_mapping(impl->map);
	if (impl->net.is_connected()) {
		impl->net.sync_start();
		impl->es.add_filter(impl->ef);
	}
	os::time::reset_rel_time();
	impl->es.enable();
}

void GameMgr::record_game(bool b)
{
	if (b)
		impl->es.add_filter(os::event::make_savedevice());
}

void GameMgr::playback_game(const string& id)
{
	if (!id.empty())
		impl->es.add_filter(os::event::make_playbackdevice(id));
}

void GameMgr::shutdown()
{
	misc::zero_delete(impl);
}


}
}

