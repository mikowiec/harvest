
/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:09 $
 * $Log: nameservice.cpp,v $
 * Revision 1.14  2003/01/06 12:42:09  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.13  2002/12/29 17:04:40  pstrand
 * hw/ moved to os/
 *
 * Revision 1.12  2001/04/16 22:08:17  peter
 * ...
 *
 * Revision 1.11  2001/04/10 08:46:58  peter
 * kompilatorgnäll...
 *
 * Revision 1.10  2001/03/27 18:17:27  peter
 * no message
 *
 * Revision 1.9  2001/03/25 09:21:08  peter
 * Ska vara Runnable (hw/concurrent.h)...
 * visual dålig på att kompilera om?
 *
 * Revision 1.8  2001/03/24 18:23:11  niklas
 * Kompilerade inte...
 * ändrade Runnable till runnable
 *
 * Revision 1.7  2001/03/21 11:22:38  peter
 * *** empty log message ***
 *
 * Revision 1.6  2001/02/08 15:19:01  peter
 * except.
 *
 * Revision 1.5  2001/01/07 13:05:27  peter
 * *** empty log message ***
 *
 *
 */

#include "os/compat.h"

#include <string>
#include <cstdio>

#include "os/net/netcompat.h"

#ifndef WIN32
#include <netdb.h>
#endif

#include "os/net/socket.h"
#include "os/concurrent.h"
#include "net/nameservice.h"

namespace reaper
{
namespace net
{

using namespace reaper::os::net;
using namespace reaper::os::concurrent;

class NSHelper : public Runnable {
public:
	Thread* thread;
	NSHelper() {
		thread = new Thread(this);
		lq_mutex.lock();
		thread->start();
	}
	~NSHelper() { }
	Semaphore finished;
	Semaphore lq_sem;
	Mutex lq_mutex;

	std::string host;
	addr_t addr;
	NameData result;

	void run() {
		os::net::Net_Init net_init;
		struct hostent* hp;
		std::string h;
		addr_t a;
		lq_mutex.unlock();
		while (1) {
			lq_sem.wait();
			h = host;
			a = addr;
			lq_mutex.unlock();

			result.aliases.clear();
			result.addrs.clear();

			if (a == 0) {
				hp = gethostbyname(h.c_str());
			} else {
				hp = gethostbyaddr((char*)&a, 4, AF_INET);
			}
			if (!hp) {
				result.err = true;
				result.name = dnsstrerror();
			} else {
				result.err = false;
				result.name = hp->h_name;
				char **p = hp->h_aliases;
				while (*p) {
					result.aliases.push_back(*p);
					p++;
				}
				p = hp->h_addr_list;
				while (*p) {
					result.addrs.push_back(*((long*)*p));
					p++;
				}
			}
			finished.signal();
		}
	}
};

NameService::NameService() {
	helper = new NSHelper();
}


NameService* NameService::Instance() {
	static NameService* ns = new NameService();
	return ns;
}


bool NameService::Result(bool wait, NameData& nd) {
	if (wait) {
		helper->finished.wait();
		if (helper->result.err)
			throw network_error(helper->result.name);
		nd = helper->result;
		return true;
	} else {
		if (helper->finished.try_wait()) {
			if (helper->result.err)
				throw network_error(helper->result.name);
			nd = helper->result;
			return true;
		}
		return false;
	}
}

void NameService::Lookup(addr_t a) {
	helper->lq_mutex.lock();
	helper->host = "";
	helper->addr = a;
	helper->lq_sem.signal();
}


void NameService::Lookup(const std::string& s) {
	helper->lq_mutex.lock();
	helper->host = s;
	helper->addr = 0;
	helper->lq_sem.signal();
}


}
}
