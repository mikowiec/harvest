#ifndef REAPER_HW_NETCOMPAT_H
#define REAPER_HW_NETCOMPAT_H


#ifdef WIN32
#include "os/windows.h"
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

#ifdef CYGWIN
#include <winsock2.h>
#endif

#include "os/net/socket.h"

namespace reaper
{
namespace os
{
namespace net
{

	
std::string dnsstrerror();
std::string sockstrerror(int err = 0);

#ifdef WIN32

#define NETERR(x)	(x == SOCKET_ERROR)

typedef int socklen_t;
typedef int int_arg;

inline
int setsockopt(int s, int level, int optname, int* optval, int optlen)
{
	const char* p = reinterpret_cast<const char*>(optval);
	return ::setsockopt(s, level, optname, p, optlen);
}

#else

inline void closesocket(socket_t s) {
	close(s);
}
#define NETERR(x)	(x < 0)
typedef unsigned int int_arg;

#endif

}
}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:13 $
 * $Log: netcompat.h,v $
 * Revision 1.2  2003/01/06 12:42:13  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.8  2001/11/27 13:11:00  peter
 * hata visual c+
 *
 * Revision 1.7  2001/08/06 12:16:23  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.6.4.1  2001/08/03 13:43:59  peter
 * pragma once obsolete...
 *
 * Revision 1.6  2001/07/06 01:47:21  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.5  2001/04/29 13:47:46  peter
 * windows...
 *
 * Revision 1.4  2001/02/17 15:59:05  peter
 * vc fix.. mm..
 *
 * Revision 1.3  2001/01/05 01:36:34  peter
 * no message
 *
 */

