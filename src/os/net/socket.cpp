
/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:13 $
 * $Log: socket.cpp,v $
 * Revision 1.2  2003/01/06 12:42:13  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.24  2002/03/19 20:41:18  pstrand
 * networking
 *
 * Revision 1.23  2001/11/26 02:20:11  peter
 * ljudfixar (smartptr), andra mindre fixar..
 *
 * Revision 1.22  2001/03/12 03:11:50  peter
 * nätverksspel, funkar hjälpligt...
 *
 * Revision 1.21  2001/03/11 15:09:19  peter
 * nätverksbök
 *
 * Revision 1.20  2001/03/06 13:43:06  peter
 * *** empty log message ***
 *
 * Revision 1.19  2001/02/17 15:33:49  peter
 * *** empty log message ***
 *
 * Revision 1.18  2001/02/08 15:18:48  peter
 * *** empty log message ***
 *
 * Revision 1.17  2001/02/04 23:01:05  peter
 * no message
 *
 * Revision 1.16  2001/02/04 14:46:48  peter
 * select
 *
 * Revision 1.15  2001/01/31 22:41:13  peter
 * ...
 *
 * Revision 1.14  2001/01/30 22:54:20  peter
 * eventtjafs
 *
 * Revision 1.13  2001/01/16 06:33:00  peter
 * fixar...
 *
 * Revision 1.12  2001/01/05 01:36:34  peter
 * no message
 *
 * Revision 1.11  2001/01/01 15:31:40  peter
 * getfd()
 *
 */


#include "os/compat.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <string> 
#include <iostream>
#include <deque>

#include "os/net/netcompat.h"
#include "os/debug.h"
#include "misc/parse.h"
#include "os/net/socket.h"

#ifndef WIN32

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#endif


///
namespace reaper
{
///
namespace os
{
///
namespace net
{

namespace { reaper::debug::DebugOutput derr("sockstream"); }

typedef struct sockaddr_in sa_in;
typedef struct sockaddr* sa_ptr;

#ifdef WIN32

#pragma comment(lib, "wsock32.lib")

Net_Init::Net_Init() {
	if (++inst > 1)
		return;
	WSADATA dt;
	int error;
	if ((error = WSAStartup(0x0101, &dt)) != 0) {
		throw socket_error(sockstrerror(error));
	}
}

Net_Init::~Net_Init() {
	if (--inst == 0)
		WSACleanup();
}

int Net_Init::inst = 0;

#else

Net_Init::Net_Init() { }
Net_Init::~Net_Init() { }

#endif



class ConnData {
public:
	std::string r_name;
	struct sockaddr_in l_addr, r_addr;
	Socket::Proto proto;
	Net_Init net_init;

	ConnData() {
		reset();
	}
	void reset()
	{
		l_addr.sin_family = r_addr.sin_family = AF_INET;
		l_addr.sin_addr.s_addr = r_addr.sin_addr.s_addr = 0;
		l_addr.sin_port = r_addr.sin_port = 0;
		proto = Socket::TCP;
	}
};

Socket::Socket()
{
	cdata = new ConnData;
	initsocket();
}

Socket::Socket(const Socket &sock)
{
	cdata = new ConnData;
	cdata->r_name = sock.cdata->r_name;
	cdata->l_addr = sock.cdata->l_addr;
	cdata->r_addr = sock.cdata->r_addr;
	cdata->proto = sock.cdata->proto;
	initsocket();
}

void sock_err(const std::string& err) {
	throw socket_error(err + ": " + sockstrerror());
}


Socket::Socket(socket_t s) {
	cdata = new ConnData;
	sock = s;
	int i = sizeof(sockaddr_in);
	if (NETERR(getsockname(sock, (sa_ptr)&cdata->l_addr, (int_arg*)&i)))
		sock_err("getsockname");
	if (NETERR(getpeername(sock, (sa_ptr)&cdata->r_addr, (int_arg*)&i)))
		sock_err("getsockname");
}

Socket::~Socket()
{
	delete cdata;
	closesocket(sock);
}

int Socket::getfd()
{
	return sock;
}

void Socket::initsocket(socket_t s) {
	if (s != 0)
		sock = s;
	else {
		int p = (cdata->proto == TCP) ? SOCK_STREAM : SOCK_DGRAM;
		if (!(sock = socket(PF_INET, p, 0))) {
			sock_err("initsocket");
		}
	}
}

void Socket::connect(addr_t addr, long port, Proto proto)
{
	set_proto(proto);
	if (sock == 0)
		initsocket();
	if (port != 0)
		cdata->r_addr.sin_port = htons(port);
	if (addr != 0)
		cdata->r_addr.sin_addr.s_addr = addr;
	if (NETERR(::connect(sock, (sa_ptr)&cdata->r_addr, sizeof(sa_in))))
		sock_err("connect");
	int_arg i = sizeof(sa_in);
	if (NETERR(getsockname(sock, (sa_ptr)&cdata->l_addr, (int_arg*)&i)))
		sock_err("getsockname");
}

void Socket::listen(long port)
{
	if (port != 0)
		cdata->l_addr.sin_port = htons(port);
	 int v = 1;
	 if (NETERR(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v)))) {
		sock_err("setsockopt");
	 }
	if (NETERR(bind(sock, (sa_ptr)&cdata->l_addr, sizeof(sa_in))))
		sock_err("bind");
	int_arg i = sizeof(cdata->l_addr);
	if (NETERR(getsockname(sock, (sa_ptr)&cdata->l_addr, (int_arg *)&i)))
		sock_err("getsockname");
	if (cdata->proto == TCP) {
		if (NETERR(::listen(sock, 10)))
			sock_err("listen");
	}
}

Socket *Socket::accept()
{
	int sock2;
	sa_in naddr;

	int_arg i = sizeof(naddr);
	if ((sock2 = ::accept(sock, (sa_ptr)&naddr, (int_arg*)&i)) == -1)
		sock_err("accept");
	return new Socket(sock2);
}

addr_t Socket::local_addr() {
	return cdata->l_addr.sin_addr.s_addr;
}

addr_t Socket::remote_addr() {
	return cdata->r_addr.sin_addr.s_addr;
}

long Socket::remote_port() {
	return ntohs(cdata->r_addr.sin_port);
}

void Socket::close()
{
	closesocket(sock);
	cdata->reset();
	sock = 0;
}

int Socket::send(const char* buf, size_t len) {
	if (cdata->proto == TCP) {
		return ::send(sock, buf, len, 0);
	} else {
		return ::sendto(sock, buf, len, 0,
				(sa_ptr) &cdata->r_addr, sizeof(sa_in));
	}
}

int Socket::recv(char* buf, size_t len, bool block) {
	if (cdata->proto == TCP) {
		return ::recv(sock, buf, len, 0);
	} else {
		socklen_t n = sizeof(sa_in);
#ifdef WIN32
		if (block)
			return ::recvfrom(sock, buf, len, 0,
						(sa_ptr)&cdata->r_addr, &n);
		else {
			unsigned long p = 1;
			ioctlsocket(sock, FIONBIO, &p);
			int r = ::recvfrom(sock, buf, len, 0,
						 (sa_ptr)&cdata->r_addr, &n);
			p = 0;
			ioctlsocket(sock, FIONBIO, &p);
			return r;
		}
#else
		return ::recvfrom(sock, buf, len, block ? 0 : MSG_DONTWAIT,
						  (sa_ptr)&cdata->r_addr, &n);
#endif
	}
}

int Socket::send(const char* buf, size_t len, addr_t addr, long port) {
	cdata->r_addr.sin_addr.s_addr = addr;
	cdata->r_addr.sin_port = htons(port);
	return send(buf, len);
}

int Socket::recv(char* buf, size_t len, addr_t* addr, long* port, bool block) {
	int r = recv(buf, len, block);
	*addr = cdata->r_addr.sin_addr.s_addr;
	*port = ntohs(cdata->r_addr.sin_port);
	return r;
}


void Socket::set_proto(Proto proto) {
	if (proto != cdata->proto) {
		cdata->proto = proto;
		closesocket(sock);
		initsocket();
	}
}

std::string addr2string(addr_t a)
{
	in_addr addr;
	addr.s_addr = a;
	return inet_ntoa(addr);
}

using std::max;



bool wait_read(const SSeq& inp, SSeq& out, SSeq& err, int timeout)
{
	fd_set read_set, write_set, exc_set;
	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	FD_ZERO(&exc_set);
	int max_fd = 0;
	for (SSeq::const_iterator c = inp.begin(); c != inp.end(); c++) {
		int fd = (*c)->getfd();
		max_fd = max(fd, max_fd);
		FD_SET(fd, &read_set);
//		FD_SET(fd, &write_set);
		FD_SET(fd, &exc_set);
	}
	struct timeval tm;
	struct timeval* to = 0;
	if (timeout != -1) {
		to = &tm;
		tm.tv_sec = timeout / 1000;
		tm.tv_usec = (timeout % 1000) * 1000;
	}
		
	int n = ::select(max_fd + 1, &read_set, &write_set, &exc_set, 0);
	if (n == -1) {
		return false;
	}
	SSeq::const_iterator c = inp.begin();
	while (n) {
		if (FD_ISSET((*c)->getfd(), &read_set)) {
			n--;
			out.push_back(*c);
		}
//		if (FD_ISSET((*c)->getfd(), &write_set)) {
//			n--;
//			out.push_back(*c);
//		}
		if (FD_ISSET((*c)->getfd(), &exc_set)) {
			n--;
			err.push_back(*c);
		}
		c++;
	}
	return err.empty();
}

}
}
}

