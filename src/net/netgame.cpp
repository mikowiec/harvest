/*
 * Network communication management.
 *
 * TODO:
 *  * split up in low- and high-level parts
 */

#include "os/compat.h"

#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <functional>

#include "os/debug.h"
#include "msg/event.h"
#include "os/abstime.h"
#include "os/reltime.h"
#include "os/worker.h"
#include "os/concurrent.h"

#include "net/net.h"
#include "misc/iostream_helper.h"
#include "misc/sequence.h"
#include "res/config.h"
#include "net/sockstream.h"

#include "object/phys.h"

#include "main/types_io.h"
#include "net/game_mgr.h"
#include "net/netgame.h"

namespace reaper
{
namespace net
{

namespace { debug::DebugOutput derr("netgame", 5); }

using os::event::Event;

using misc::crlf;
using misc::spaces;
using misc::send;
using misc::seq;
using misc::copy;

namespace cc = os::concurrent;

/*
 * getline on some systems blocks unexpectedly,
 * (probably due to bugs in my code...)
 * should investigate further at some time, workaround for now
 *  - pstrand
 */
void getline(std::istream& is, char* buf = 0, int max = 100)
{
	char junk[100];
	char *p = (buf) ? buf : junk;

	int c = 0;
	while (max-- && (c = is.get()) != -1) {
		if (c == '\r' || c == '\n') {
			int c2 = is.get();
			if (c2 != '\n')
				is.putback(c2);
			*p = '\0';
			return;
		}
		*p++ = c;
	}
	*p = '\0';
}


class ServerTalk
 : public os::worker::Job
{
	cc::Mutex& sync_mtx;
	cc::Mutex in_mtx, out_mtx, obj_mtx;
	cc::Semaphore die, dead;

	typedef std::deque<Event> event_queue;
	event_queue in_queue, out_queue;
	bool obj_sync_put, obj_sync_get;

	StateReps put_obj_states, get_obj_states;

	sock_stream* conn;

	// Maps event-handing ids to joined ids
	std::map<PlayerID,PlayerID> local_map;

	long event_delay;
public:
	ServerTalk(cc::Mutex& mtx)
	 : sync_mtx(mtx),
	   obj_sync_put(false), obj_sync_get(false),
	   conn(0), event_delay(0x100)
	{
		local_map[0] = 0;
	}
	~ServerTalk() { }

	void set_stream(sock_stream* ss) { conn = ss; }

	void map_id(PlayerID from, PlayerID to) {
		local_map[from] = to;
	}

	void ext_send()
	{
		event_queue out_buf;
		{
			cc::ScopeLock lo(out_mtx);
			out_buf.swap(out_queue);
		}
		cc::ScopeLock lc(sync_mtx);
		while (! out_buf.empty()) {
			*conn << "event " << out_buf.front() << "\r\n";
//			derr << "ext_send: " << out_buf.front() << '\n';
			out_buf.pop_front();
		}
	}

	void ext_poll()
	{
		Event ev;
		event_queue in_buf;
		cc::ScopeLock lc(sync_mtx);
		net::send(*conn, "poll");
		while (conn->good() && conn->peek() != 'e') {
			*conn >> ev >> crlf;
//			derr << "ext_poll: " << ev << '\n';
			in_buf.push_back(ev);
		}
		std::string buf;
		*conn >> buf >> crlf;
		cc::ScopeLock li(in_mtx);
		copy(seq(in_buf), std::back_inserter(in_queue));
	}

	void obj_dosync()
	{
		if (obj_sync_put) {
			StateReps put_objs;
			{
				cc::ScopeLock lo(obj_mtx);
				put_objs.swap(put_obj_states);
			}
			cc::ScopeLock lc(sync_mtx);
			while (conn->good() && !put_objs.empty()) {
				*conn << "putobj " << put_objs.front() << "\r\n";
				put_objs.pop_front();
			}

			obj_sync_put = false;
		}

		if (obj_sync_get) {
			cc::ScopeLock lc(sync_mtx);
			net::send(*conn, "getobjs");

			StateReps get_objs;
			while (conn->good() && conn->peek() != 'e') {
				char buf[500];
				net::getline(*conn, buf, 500);
				get_objs.push_back(std::string(buf));
			}
			std::string junk;
			*conn >> junk >> crlf;

			cc::ScopeLock lo(obj_mtx);
			copy(seq(get_objs), std::back_inserter(get_obj_states));
			obj_sync_get = false;
		}
	}

	bool operator()()
	{
		if (conn) {
			obj_dosync();
			ext_send();
			ext_poll();
			if (!conn->good()) {
				conn = 0;
			}
		}
		if (die.try_wait()) {
			in_queue.clear();
			out_queue.clear();
			dead.signal();
			return false;
		} else {
			return true;
		}
	}

	void kill()
	{
//		derr << "netgame dying\n";
		die.signal();
//		derr << "waiting\n";
		dead.wait();
//		derr << "Finally dead!\n";
	}

	bool send(const Event& e)
	{
		bool send_local = false;
		bool send_remote = false;

		if (e.recv == os::event::System)
			send_local = true;
		else if (local_map.find(e.recv) != local_map.end())
			send_local = send_remote = true;
		else
			send_remote = true;

		Event ev(e);
		ev.recv = local_map[e.recv];
		ev.time += event_delay;
		if (send_local) {
			cc::ScopeLock li(in_mtx);
//			derr << "send (local): " << ev << '\n';
			in_queue.push_back(ev);
		}
		if (send_remote) {
			cc::ScopeLock lo(out_mtx);
//			derr << "send (remote): " << ev << '\n';
			out_queue.push_back(ev);
		}
		return true;
	}

	bool poll(Event& e)
	{
		cc::ScopeLock li(in_mtx);
		if (in_queue.empty())
			return false;
		e = in_queue.front();
		in_queue.pop_front();
//		derr << "poll: " << e << '\n';
		return true;
	}

#if 0
	void put_obj(const ObjState& st)
	{
		cc::ScopeLock lo(obj_mtx);

		put_obj_states.push_back(st);
		obj_sync_put = true;
	}

	bool get_obj(ObjState& st)
	{
		cc::ScopeLock lo(obj_mtx);
		obj_sync_get = true;
		if (get_obj_states.empty())
			return false;
		st = get_obj_states.front();
		get_obj_states.pop_back();
		return true;
	}
#endif

	void get_objstates(StateReps& rep)
	{
		cc::ScopeLock lo(obj_mtx);
		// swap instead?
		misc::copy(misc::seq(get_obj_states), std::back_inserter(rep));
		get_obj_states.clear();
		obj_sync_get = true;
	}

	void put_objstates(const StateReps& rep)
	{
		cc::ScopeLock lo(obj_mtx);
		misc::copy(misc::cseq(rep), std::back_inserter(put_obj_states));
		obj_sync_put = true;
	}
};

class NetFwd : public os::event::EventFilter
{
	ServerTalk* srv_talk;
	os::event::EventSource* src;
public:
	NetFwd(ServerTalk* s) : srv_talk(s) { }

	void set_source(os::event::EventSource* s) { src = s; }

	bool poll(Event& e)
	{
		Event ev;
		while (src->poll(ev)) {
			srv_talk->send(ev);
		}
		return srv_talk->poll(e);
	}

};

NetGame::NetGame()
 : conn(0), srv_talk(0), next_id(1)
{
}

NetGame::~NetGame()
{
	shutdown();
	delete srv_talk;
}

std::string talk(std::iostream& io, const std::string& s)
{
	char buf[100];
	send(io, s);
//	derr << "Sent: |" << s << "|\n";
	getline(io, buf, 100);
//	derr << "Got: |" << buf << "|\n";
	return buf;
}


os::event::EventFilter* NetGame::connect(std::string srv)
{
	misc::stringpair server = misc::split(srv, ':');
	int port = server.second.empty()
			? 4247
			: read<int>(server.second);
	derr << "Connecting to " << server.first << ':' << port << '\n';

	conn = new sock_stream(server.first, port);
	char buf[200];

	getline(*conn, buf, 200);
	derr << "Conn msg: " << buf << '\n';

	net::send(*conn, "time");
	os::time::TimeSpan srv_time;
	*conn >> srv_time >> crlf;

	derr << "time before " << os::time::get_time()
	     << "  server time " << srv_time;
	os::time::set_time(srv_time);
	derr << "  time after " << os::time::get_time() << '\n';

	srv_talk = new ServerTalk(sync_mtx);
	srv_talk->set_stream(conn);
	os::worker::worker()->add_job(srv_talk);
	return new NetFwd(srv_talk);
}

PlayerID NetGame::join(bool observer)
{
	PlayerID id = next_id;
	if (!conn) {
		game.players.insert(id);
	} else {
		cc::ScopeLock sc(sync_mtx);
		if (observer) {
			net::send(*conn, "obs");
			std::string junk;
			*conn >> junk >> crlf;
			srv_talk->map_id(next_id, -1);
		} else {
			net::send(*conn, "join");
			*conn >> id >> crlf;
			srv_talk->map_id(next_id, id);
		}
	}
	++next_id;
	return id;
}

bool NetGame::start_net_game()
{
	cc::ScopeLock sc(sync_mtx);
	*conn << "start\r\n" << std::flush;
	std::string junk;
	*conn >> junk >> crlf;
	derr << "start_net_game " << junk << '\n';
	return junk == "ack";
}

bool NetGame::sync_start()
{
	cc::ScopeLock sc(sync_mtx);
	*conn << "go\r\n" << std::flush;
	std::string junk;
	*conn >> junk >> crlf;
	return junk == "ack";
}

bool NetGame::get_multistatus()
{
	cc::ScopeLock sc(sync_mtx);
	net::send(*conn, "gameinfo");

	game.players.clear();

	*conn >> game.state >> crlf;
	while (conn->peek() != 'e') {
		PlayerID id;
		*conn >> id >> crlf;
		game.players.insert(id);
	}
	std::string buf;
	*conn >> buf >> crlf;
	return game.state == startup;
}

void NetGame::shutdown()
{
	derr << "NetGame::shutdown\n";
	if (srv_talk)
		srv_talk->kill();
	if (conn) {
		cc::ScopeLock sc(sync_mtx);
		send(*conn, "quit");

		srv_talk->set_stream(0);
		delete conn;
		conn = 0;
	}
}

const Players& NetGame::players() const
{
	return game.players;
}

bool NetGame::is_connected() const
{
	return conn != 0;
}

/*
os::time::RelTime NetGame::until_start() const
{
	return static_cast<os::time::RelTime>(
		(game.start_time - os::time::get_time()).approx() / 1000.0);
}
*/

#if 0
void NetGame::send_objinfo(const ObjState& st)
{
	if (srv_talk)
		srv_talk->put_obj(st);
}

bool NetGame::get_objinfo(ObjState& st)
{
	return (srv_talk) ? (srv_talk->get_obj(st)) : false;
}
#endif

void NetGame::get_objstates(StateReps& rep)
{
	if (srv_talk)
		srv_talk->get_objstates(rep);	
}

void NetGame::put_objstates(const StateReps& rep)
{
	if (srv_talk)
		srv_talk->put_objstates(rep);
}

}
}

