#ifndef REAPER_NET_SOCKSTREAM_H
#define REAPER_NET_SOCKSTREAM_H

#include <iostream>
#include <string>

#include "os/net/socket.h"

namespace reaper
{
namespace net
{

using reaper::os::net::Socket;

class sock_streambuf
 : public std::basic_streambuf<char>
{
	Socket* sock;
	bool sock_is_mine;
	char ibuf[512];
	char obuf[512];
	void init();
	int read_some();
	int write_some();
public:
	sock_streambuf(const std::string& host, int port);
	sock_streambuf(Socket* s);
	~sock_streambuf();

	typedef std::char_traits<char> ct;
	ct::int_type overflow(ct::int_type c);
	virtual ct::int_type underflow();
	virtual ct::int_type uflow();
	virtual ct::int_type pbackfail(ct::int_type c);
	virtual int sync();
	std::streamsize xsgetn(ct::char_type* s, std::streamsize n);
	std::streamsize xsputn(const ct::char_type* s, std::streamsize n);
	bool more() const;
};


class sock_stream
 : public std::iostream
{
	sock_streambuf sock_buf;
public:
	sock_stream(const std::string& host, int port);
	sock_stream(Socket* sock);
	bool more() const { return sock_buf.more(); }
};


}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:10 $
 * $Log: sockstream.h,v $
 * Revision 1.15  2003/01/06 12:42:10  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.14  2002/12/29 17:04:40  pstrand
 * hw/ moved to os/
 *
 * Revision 1.13  2002/08/12 11:38:43  pstrand
 * *** empty log message ***
 *
 * Revision 1.12  2001/08/06 12:16:34  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.11.4.1  2001/08/03 13:44:09  peter
 * pragma once obsolete...
 *
 * Revision 1.11  2001/07/06 01:47:29  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.10  2001/05/12 19:48:35  peter
 * sync..
 *
 * Revision 1.9  2001/03/26 10:45:11  peter
 * dödade lite minnesläckor..
 *
 */


