/*
 * iostream for network communication, uses hw/socket.cpp
 */

#include "os/compat.h"
#include "os/debug.h"
#include "net/sockstream.h"
#include "net/nameservice.h"

#include <iostream>

#include <errno.h>

namespace reaper
{
namespace net
{

namespace { reaper::debug::DebugOutput derr("sockstream", 5); }

typedef std::char_traits<char> ct;



sock_streambuf::sock_streambuf(const std::string& host, int port)
 : sock_is_mine(true)
{
	init();
	NameData nd;
	NameService* ns = NameService::Instance();
	ns->Lookup(host);
	ns->Result(true, nd);
	sock = new Socket();
	sock->connect(nd.addrs.front(), port);
}

sock_streambuf::sock_streambuf(Socket* s)
 : sock_is_mine(false)
{
	init();
	sock = s;
}

sock_streambuf::~sock_streambuf()
{
	if (sock_is_mine)
		delete sock;
}

void sock_streambuf::init()
{
	setg(ibuf + 4, ibuf + 4, ibuf + 4);
	setp(obuf, obuf + sizeof(obuf));
}

int sock_streambuf::read_some()
{
//	derr << "read_some:" << (void*)ibuf << ' ';
//	derr << (void*)gptr() << ' ' << (void*)egptr() << '\n';
	int num_pb = std::min(int(gptr() - eback()), 4);

	memmove(ibuf + (4-num_pb), gptr() - num_pb, num_pb);

	int n = sock->recv(ibuf + 4, 512 - 4);
//	derr << "read |" << std::string(ibuf + 4, n) << "|\n";

	if (n <= 0) {
		setg(0,0,0);
		return -1;
	} else {
		setg(ibuf + 4-num_pb, ibuf + 4, ibuf + 4 + n);
//		derr << "read end: " << n << ' ' << (void*)gptr() << ' ' << (void*)egptr() << '\n';
		return n;
	}
}



ct::int_type sock_streambuf::underflow()
{
//	derr << "underflow: " << (void*)ibuf << ' ' << (void*)gptr() << ' ' << (void*)egptr() << '\n';
	if (gptr() < egptr())
		return ct::to_int_type(*gptr());
	
	if (read_some() < 0)
		return ct::eof();
	else
		return ct::to_int_type(*gptr());
}

ct::int_type sock_streambuf::uflow()
{
	ct::int_type c = underflow();
	if (!ct::eq_int_type(c, ct::eof()))
		gbump(1);
	return c;
}

ct::int_type sock_streambuf::pbackfail(ct::int_type c)
{
	if (gptr() != eback()) {
		gbump(-1);
		if (ct::eq_int_type(c, ct::eof()))
			*(gptr()) = ct::to_char_type(c);
		return ct::not_eof(c);
	} else
		return ct::eof();		
}

bool sock_streambuf::more() const
{
//	derr << "more " << (gptr() < egptr()) << '\n';
	return gptr() < egptr();
}

int sock_streambuf::write_some()
{
//	derr << "write_some\n";

	int cnt = pptr() - pbase();
	int n = sock->send(pbase(), cnt);
//	derr << "write |" << std::string(obuf, cnt) << "|" << n << "\n";
	pbump(-cnt);
	return n;
}

ct::int_type sock_streambuf::overflow(ct::int_type c)
{
//	derr << "overflow\n";

	if (write_some() < 0)
		return ct::eof();
	else {
		if (!ct::eq_int_type(c, ct::eof()))
			return sputc(c);
		else
			return ct::not_eof(c);
	}
}

int sock_streambuf::sync()
{
//	derr << "sync\n";
	return !write_some();
}

std::streamsize sock_streambuf::xsgetn(ct::char_type* s, std::streamsize n)
{
	std::streamsize i = 0;
	while (i < n) {
		ct::int_type c = uflow();
		if (ct::eq_int_type(c, ct::eof()))
			return i;
		*s = ct::to_char_type(c);
		++i;
	}
	return i;
}


std::streamsize sock_streambuf::xsputn(const ct::char_type *s, std::streamsize n)
{
//	derr << "xsputn\n";
	if (n < epptr() - pptr()) {
		memcpy(pptr(), s, n);
		pbump(n);
		return n;
	} else {
		for (std::streamsize i = 0; i < n; i++) {
			if (ct::eq_int_type(sputc(s[i]), ct::eof()))
				return i;
		}
		return n;
	}
}

typedef std::iostream std_iostream;

sock_stream::sock_stream(const std::string& host, int port)
 : std_iostream(0)
 , sock_buf(host, port)
{
	init(&sock_buf);
}

sock_stream::sock_stream(Socket* sock)
 : std_iostream(0)
 , sock_buf(sock)
{
	init(&sock_buf);
}


}
}

