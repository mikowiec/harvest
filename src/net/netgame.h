

#ifndef NET_NETGAME_H
#define NET_NETGAME_H

#include "os/abstime.h"
#include "os/reltime.h"
#include "net/net.h"

namespace reaper {
namespace os {
	namespace event { class EventFilter; }
}
namespace net {

class sock_stream;
class ServerTalk;


struct GameInfo
{
	GameInfo() { }
	PlayerID local_id;
	Players players;
	GameState state;
	os::time::TimeSpan start_time;
};



class NetGame
{
	sock_stream* conn;
	GameInfo game;
	ServerTalk* srv_talk;
	os::concurrent::Mutex sync_mtx;
	PlayerID my_id, next_id;
public:
	NetGame();
	~NetGame();

	os::event::EventFilter* connect(std::string srv);
	void shutdown();

	PlayerID join(bool observer);
	bool start_net_game();

	bool get_multistatus();
	const Players& players() const;
	bool is_connected() const;
//	os::time::RelTime until_start() const;
	bool sync_start();

//	void send_objinfo(const ObjState& st);
//	bool get_objinfo(ObjState& st);

	void get_objstates(StateReps&);
	void put_objstates(const StateReps&);
};



}
}

#endif

