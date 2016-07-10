#ifndef REAPER_HW_SOCKET_H
#define REAPER_HW_SOCKET_H

#include "os/exceptions.h"

#include <deque>

///
namespace reaper
{
///
namespace os
{
/// Lowlevel sockets
namespace net
{

/** Exception thrown at error.
    contains errordescription:
     strerror(errno) under linux, 
     WSAXXXX under windows
 */
struct socket_error : public hw_error {
	socket_error(const std::string& s) : hw_error(s) { }
};

/*
struct timeout : public hw_error {
	timeout() : hw_error("network timeout") { }
};
*/

/// Socket handle. will probably disappear in the future
typedef int socket_t;

/// Address in internal representation. do not try to use directly
typedef long addr_t;

/** Show addr_t.
    Translate an (IP) address to string representation, ie: NN.NN.NN.NN
*/
std::string addr2string(addr_t a);


class Net_Init {
	static int inst;
public:
	Net_Init();
	~Net_Init();
};

class ConnData;


/** Socket.
    TCP (default) or UDP socket.
*/
class Socket {
	socket_t sock;
	ConnData* cdata;
	void initsocket(socket_t s = 0);
	Socket(socket_t);
public:
	int getfd();
	/// Protocol.
	enum Proto {TCP, UDP};

	Socket();
	Socket(const Socket& sock);
	~Socket();

	/// Change protocol. Changes protocol used for next connection.
	void set_proto(Proto proto);

	/** Set local address and/or port.
	    Sets the value if non-zero. Only useful before binding
	    an listening socket.
	*/
	void set_local_params(addr_t addr, long port);

	/** Set remote address and/or port.
	    Sets the value if non-zero. 
	    If both are set, connect can be called without arguments.
	*/
	void set_remote_params(addr_t addr, long port);


	/// Get local address
	addr_t local_addr();

	/// Get remote address
	addr_t remote_addr();

	/// Get remote port
	long remote_port();

	/** Connect to remote host using (optinal) addr/port/proto.
	    The arguments are only used if non-zero.
	*/
	void connect(addr_t addr = 0, long port = 0, Proto proto = TCP);

	/// Listen on a specific port (only TCP)
	void listen(long port);

	/// Accept connection (only TCP)
	Socket* accept();

	/// Close connection (only TCP)
	void close();

	/// Send data on socket
	int send(const char* buf, size_t len);

	/// Send data to specific reciever (only UDP).
	int send(const char* buf, size_t len, addr_t addr, long port);

	/// Receive data. Will block unless \b block is \b false (only UDP)
	int recv(char* buf, size_t len, bool block = true);

	/** Receive data.
	    Gets address and port of sender,
	    will block unless \b block is \b false#(only UDP)
	*/
	int recv(char* buf, size_t len,
		 addr_t* addr, long* port, bool block = true);

};

typedef std::deque<Socket*> SSeq;

bool wait_read(const SSeq& in, SSeq& out, SSeq& err, int timeout = -1);


}

}
}

#endif


/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:13 $
 * $Log: socket.h,v $
 * Revision 1.2  2003/01/06 12:42:13  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.20  2002/03/19 20:41:18  pstrand
 * networking
 *
 * Revision 1.19  2002/02/21 15:25:27  peter
 * speling
 *
 * Revision 1.18  2001/08/06 12:16:24  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.17.4.1  2001/08/03 13:44:00  peter
 * pragma once obsolete...
 *
 * Revision 1.17  2001/07/06 01:47:22  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.16  2001/04/18 23:45:35  peter
 * *** empty log message ***
 *
 * Revision 1.15  2001/03/11 15:09:19  peter
 * nätverksbök
 *
 * Revision 1.14  2001/02/08 15:18:48  peter
 * *** empty log message ***
 *
 */

