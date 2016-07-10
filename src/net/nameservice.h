#ifndef REAPER_NET_NAMESERVICE_H
#define REAPER_NET_NAMESERVICE_H

#include <list>

#include "os/net/socket.h"

#include "main/exceptions.h"

/// Reaper!
namespace reaper
{
/// Network
namespace net
{

using namespace reaper::os::net;

/// Result of nameservice query.
struct NameData {
	bool err;			/**< Error? if so, name contains error
					    description (this will change...) */
	std::string name;		///< Primary name
	std::list<addr_t> addrs;	///< IP-addresses
	std::list<std::string> aliases;	///< Aliases
};

class NSHelper;

/// Domainname lookup service;
class NameService {
	NSHelper* helper;
	NameService();
public:
	static NameService* Instance();
	void Lookup(const std::string& s);
	void Lookup(addr_t addr);
	bool Result(bool wait, NameData&);
};

class network_error
: public error_base
{   
protected:
	std::string err;
public:
	network_error(const std::string& s) : err(s) { }
	const char* what() const { return err.c_str(); }
	virtual ~network_error() { }
};

}
}

#endif



/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:10 $
 * $Log: nameservice.h,v $
 * Revision 1.9  2003/01/06 12:42:10  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.8  2002/12/29 17:04:40  pstrand
 * hw/ moved to os/
 *
 * Revision 1.7  2001/08/06 12:16:34  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.6.4.1  2001/08/03 13:44:09  peter
 * pragma once obsolete...
 *
 * Revision 1.6  2001/07/06 01:47:29  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.5  2001/02/08 15:19:02  peter
 * except.
 *
 * Revision 1.4  2001/01/07 13:05:27  peter
 * *** empty log message ***
 *
 *
 *
 */

