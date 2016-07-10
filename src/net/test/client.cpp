/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:10 $
 * $Log: client.cpp,v $
 * Revision 1.5  2003/01/06 12:42:10  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.4  2002/12/29 17:04:40  pstrand
 * hw/ moved to os/
 *
 * Revision 1.3  2001/03/04 14:24:36  peter
 * gfx config
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

#include <string>
#include <vector>
#include <iostream>

#include "os/compat.h"

#include "os/socket.h"
#include "net/nameservice.h"
#include "os/fdstream.h"
#include "net/sockstream.h"
#include "os/time.h"

using namespace reaper::hw;
using namespace reaper::net;
using reaper::os::time::get_time;

int main(int argc, char *argv[]) {
	NameData nd;
	std::vector<int> times;
	Socket s;
	NameService::Instance()->Lookup(argc == 2 ? argv[1] : "localhost");
	NameService::Instance()->Result(true, nd);
	char buf[512];
	long t1, t2;
	strcpy(buf, "hello, world");
	try {
		addr_t a = nd.addrs.front();
		s.connect(a, 4242);
		sock_stream ss(&s);
		ss << "Hello world\n" << std::flush;
		ss >> buf;
		std::cout << "->" << buf << "<-\n";
	} catch (socket_error& e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}

