/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:13 $
 * $Log: netcompat.cpp,v $
 * Revision 1.2  2003/01/06 12:42:13  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.3  2001/01/05 01:36:34  peter
 * no message
 *
 */


#include "os/compat.h"

#include <string>

#include "os/net/netcompat.h"


namespace reaper
{
namespace os
{
namespace net
{


#ifdef WIN32

std::string dnsstrerror()
{
	return sockstrerror();
}

std::string sockstrerror(int err)
{
	if (! err)
		err = WSAGetLastError();
	switch (err) {
	case WSAEINTR:			return "WSAEINTR";
	case WSAEBADF:			return "WSAEBADF";
	case WSAEACCES:			return "WSAEACCES";
	case WSAEFAULT:			return "WSAEFAULT";
	case WSAEINVAL:			return "WSAEINVAL";
	case WSAEMFILE:			return "WSAEMFILE";
	case WSAEWOULDBLOCK:		return "WSAEWOULDBLOCK";
	case WSAEINPROGRESS:		return "WSAEINPROGRESS";
	case WSAEALREADY:		return "WSAEALREADY";
	case WSAENOTSOCK:		return "WSAENOTSOCK";
	case WSAEDESTADDRREQ:		return "WSAEDESTADDRREQ";
	case WSAEMSGSIZE:		return "WSAEMSGSIZE";
	case WSAEPROTOTYPE:		return "WSAEPROTOTYPE";
	case WSAENOPROTOOPT:		return "WSAENOPROTOOPT";
	case WSAEPROTONOSUPPORT:	return "WSAEPROTONOSUPPORT";
	case WSAESOCKTNOSUPPORT:	return "WSAESOCKTNOSUPPORT";
	case WSAEOPNOTSUPP:		return "WSAEOPNOTSUPP";
	case WSAEPFNOSUPPORT:		return "WSAEPFNOSUPPORT";
	case WSAEAFNOSUPPORT:		return "WSAEAFNOSUPPORT";
	case WSAEADDRINUSE:		return "WSAEADDRINUSE";
	case WSAEADDRNOTAVAIL:		return "WSAEADDRNOTAVAIL";
	case WSAENETDOWN:		return "WSAENETDOWN";
	case WSAENETUNREACH:		return "WSAENETUNREACH";
	case WSAENETRESET:		return "WSAENETRESET";
	case WSAECONNABORTED:		return "WSAECONNABORTED";
	case WSAECONNRESET:		return "WSAECONNRESET";
	case WSAENOBUFS:		return "WSAENOBUFS";
	case WSAEISCONN:		return "WSAEISCONN";
	case WSAENOTCONN:		return "WSAENOTCONN";
	case WSAESHUTDOWN:		return "WSAESHUTDOWN";
	case WSAETOOMANYREFS:		return "WSAETOOMANYREFS";
	case WSAETIMEDOUT:		return "WSAETIMEDOUT";
	case WSAECONNREFUSED:		return "WSAECONNREFUSED";
	case WSAELOOP:			return "WSAELOOP";
	case WSAENAMETOOLONG:		return "WSAENAMETOOLONG";
	case WSAEHOSTDOWN:		return "WSAEHOSTDOWN";
	case WSAEHOSTUNREACH:		return "WSAEHOSTUNREACH";
	case WSAENOTEMPTY:		return "WSAENOTEMPTY";
	case WSAEPROCLIM:		return "WSAEPROCLIM";
	case WSAEUSERS:			return "WSAEUSERS";
	case WSAEDQUOT:			return "WSAEDQUOT";
	case WSAESTALE:			return "WSAESTALE";
	case WSAEREMOTE:		return "WSAEREMOTE";
	case WSAEDISCON:		return "WSAEDISCON";
	case WSASYSNOTREADY:		return "WSASYSNOTREADY";
	case WSAVERNOTSUPPORTED:	return "WSAVERNOTSUPPORTED";
	case WSANOTINITIALISED:		return "WSANOTINITIALISED";
	case WSAHOST_NOT_FOUND:		return "WSAHOST_NOT_FOUND";
	case WSATRY_AGAIN:		return "WSATRY_AGAIN";
	case WSANO_RECOVERY:		return "WSANO_RECOVERY";
	case WSANO_DATA:		return "WSANO_DATA";
	}
	return "unknown error";
}



#else

std::string dnsstrerror() {
	return hstrerror(h_errno);
}

std::string sockstrerror(int err) {
	return strerror(err ? err : errno);
}


#endif


}
}
}
