/*
 * $Author: pstrand $
 * $Date: 2002/12/29 17:04:40 $
 * $Log: server.cpp,v $
 * Revision 1.30  2002/12/29 17:04:40  pstrand
 * hw/ moved to os/
 *
 * Revision 1.29  2001/05/12 19:58:40  peter
 * no message
 *
 * Revision 1.28  2001/05/12 07:30:49  peter
 * *** empty log message ***
 *
 * Revision 1.27  2001/05/11 18:51:01  peter
 * *** empty log message ***
 *
 * Revision 1.26  2001/05/10 11:05:20  peter
 * *** empty log message ***
 *
 * Revision 1.25  2001/05/10 00:09:23  peter
 * *** empty log message ***
 *
 * Revision 1.24  2001/05/09 23:02:30  peter
 * *** empty log message ***
 *
 * Revision 1.23  2001/05/07 23:05:30  peter
 * *** empty log message ***
 *
 * Revision 1.22  2001/05/07 16:52:59  peter
 *
 */

#include "os/compat.h"

#include "misc/test_main.h"
#include "net/server.h"


int test_main()
{
	reaper::net::GameServer server;
	server.run();
 	return 0;
}

