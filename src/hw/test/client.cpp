

#include <iostream>
#include <string>
#include <list>

#include "misc/sequence.h"
#include "os/socket.h"
#include "net/nameservice.h"
#include "os/time.h"

using namespace reaper::os::net;
using namespace reaper::os::time;
using namespace reaper::net;

int main() {
	Socket socket;
	NameData nd;
	
	NameService::Instance()->Lookup("zarquon");
	NameService::Instance()->Result(true, nd);
	socket.set_proto(Socket::TCP);

	socket.connect(nd.addrs.front(), 25);

	while (1) {
		char buf[500];
		int r;
		r = socket.recv(buf, 500);
		std::cout << std::string(buf, r) << std::endl;
		msleep(1000);
	}
}





