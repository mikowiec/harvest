#ifndef REAPER_NET_SERVER_H
#define REAPER_NET_SERVER_H


namespace reaper {
namespace net {


class Server;
struct GameServer
{
	Server* server;
public:
	GameServer();
	~GameServer();
	void run();
};


}
}

#endif

/*
 * $Author: peter $
 * $Date: 2001/08/06 12:16:34 $
 * $Log: server.h,v $
 * Revision 1.7  2001/08/06 12:16:34  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.6.4.1  2001/08/03 13:44:09  peter
 * pragma once obsolete...
 *
 * Revision 1.6  2001/07/06 01:47:29  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.5  2001/05/12 19:48:36  peter
 * sync..
 *
 * Revision 1.4  2001/05/12 16:14:55  peter
 * no message
 *
 * Revision 1.3  2001/01/23 21:37:37  peter
 * *** empty log message ***
 *
 * Revision 1.8  2001/01/11 22:58:12  peter
 * *** empty log message ***
 *
 * Revision 1.7  2001/01/07 13:05:28  peter
 * *** empty log message ***
 *
 *
 *
 */

