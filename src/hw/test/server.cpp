
#include <iostream>
#include <string>
#include <errno.h>
#include <list>
#include <queue>
#include <deque>

#include "misc/sequence.h"
#include "socket.h"

using namespace reaper::os::net;
using namespace reaper::misc;

class Client {
public:
	addr_t addr;
	long port;
	Client(addr_t a, long p) : addr(a), port(p) { }
	bool operator==(const Client& c) const {
		return c.addr == addr && c.port == port;
	}
};

ostream& operator<<(ostream& os, const Client& c) {
	os << addr2string(c.addr) << ":" << c.port;
	return os;
}


void notify(Socket s, Seq<list<Client>::iterator> cls, const string& msg) {
	while (cls) {
		cerr << "sending: " << msg << " to " << addr2string(cls->addr) << ":" << cls->port << endl;
		s.send(msg.data(), msg.size(), cls->addr, cls->port);
		cls++;
	}
}

enum { Join, Quit, Event, State, Ack };

int parsemsg(const string& s) {
	if (s == "join") return Join;
	if (s == "event") return Event;
	if (s == "state") return State;
	if (s == "ack") return Ack;
	if (s == "quit") return Quit;
	return -1;
}

void notify(Socket s, Client to, const string& msg) {
	cerr << "sending: " << msg << " to " << addr2string(to.addr) << ":" << to.port << endl;
	s.send(msg.data(), msg.size(), to.addr, to.port);	
}

int main() {
	try {
		Socket s;
		s.set_proto(Socket::UDP);
		s.listen(4242);
		list<Client> clients;
		list<pair<int, Client> > joinQ;
		queue<string> eventQ;
		list<Client>::iterator ci;


		while (1) {
			char buf[500];
			int r = s.recv(buf, 500);
			string msg(buf, r);
			Client c = Client(s.remote_addr(), s.remote_port());
			cerr << "From: " << addr2string(c.addr) << ":" << c.port << " " << msg << endl;

			switch (parsemsg(msg.substr(0,5))) {
				case Join:  joinQ.push_back(make_pair(0, c)); break;
				case Ack:
					joinQ.remove(make_pair(1, c));
					break;
				case Event: eventQ.push(msg); break;
				case State:
					msg[6] = msg[6] + 1;
					msg += " (0.0,0.0)";
					notify(s, seq(clients), msg);
					break;
				case Quit:
					clients.remove(c);
					break;
				default: cerr << "Invalid message!" << endl;
			}
			if (joinQ.size() != 0) {
				if (joinQ.front().first == 0) {
					if (clients.size() == 0) {
						notify(s, joinQ.front().second, "state 1 (0.0,0.0,0.01,0.0) (0.0,0.0)");
					} else {
						notify(s, clients.front(), "state");
					}
					ci = clients.begin();
					for (;ci != clients.end(); ci++) {
						joinQ.push_back(make_pair(1, *ci));
					}
					clients.push_back(c);
					joinQ.front().first = 1;
				}
			} else {
				while (eventQ.size()) {
					notify(s, seq(clients), eventQ.front());
					eventQ.pop();
				}
			}
		}
	} catch (socket_error& s) {
		cerr << s.errmsg << endl;
	} catch (...) {
		cerr << "Exception!" << endl;
	}
	return 0;
}



