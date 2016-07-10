#ifndef REAPER_CONC_TYPES_H
#define REAPER_CONC_TYPES_H

#ifdef PTHREADS
#include <pthread.h>
#include <semaphore.h>

#ifdef poll
# undef poll
#endif

#else

#include "os/windows.h"
#endif



namespace reaper
{
namespace os
{
namespace lowlevel
{

#ifdef PTHREADS

typedef pthread_t thread_t;
typedef pthread_mutex_t* mutex_t;
typedef sem_t* sema_t;

#else

typedef HANDLE thread_t;
typedef HANDLE mutex_t;
typedef HANDLE sema_t;

#endif

}
}
}

#endif



/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:12 $
 * $Log: conc_types.h,v $
 * Revision 1.2  2003/01/06 12:42:12  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.10  2002/09/21 17:36:10  pstrand
 * configure
 *
 * Revision 1.9  2002/01/17 04:58:42  peter
 * ljud-plugin-mm fix..
 *
 * Revision 1.8  2001/10/16 21:10:46  peter
 * video & win32 fixar
 *
 * Revision 1.7  2001/08/06 12:16:18  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.6.4.1  2001/08/03 13:43:56  peter
 * pragma once obsolete...
 *
 * Revision 1.6  2001/07/06 01:47:17  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.5  2001/04/29 13:47:56  peter
 * windows...
 *
 * Revision 1.4  2000/11/02 01:12:58  peter
 * win32.. vc.. ugh..
 *
 * Revision 1.3  2000/11/01 00:44:06  peter
 * win32
 *
 * Revision 1.2  2000/10/24 19:40:44  peter
 * solaris
 *
 * Revision 1.1  2000/09/28 10:40:30  peter
 * lite experiment....
 *
 */

