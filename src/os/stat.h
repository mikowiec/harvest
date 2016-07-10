#ifndef REAPER_HW_STAT_H
#define REAPER_HW_STAT_H

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#if defined(WIN32)

#include <direct.h>

typedef struct _stat stat_t;

#define mkdir(a,b) _mkdir(a)
#define stat(a,b) _stat(a,b)

#ifndef S_ISDIR
#define S_ISDIR(x) (_S_IFDIR & x)
#define S_ISREG(x) (_S_IFREG & x)
#endif

#ifndef S_IRWXU
#define S_IRWXU 0
#define S_IRWXG 0
#define S_IRWXO 0
#endif

#else

typedef struct stat stat_t;

#endif

#endif

/*
 * $Author: pstrand $
 * $Date: 2002/12/29 16:58:14 $
 * $Log: stat.h,v $
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.8  2002/03/12 02:22:21  pstrand
 * *** empty log message ***
 *
 * Revision 1.7  2002/03/11 10:50:48  pstrand
 * hw/time.h -> reltime.h&abstime.h, and some more..
 *
 * Revision 1.6  2002/02/13 15:44:14  peter
 * no message
 *
 * Revision 1.5  2002/01/27 03:32:00  peter
 * no message
 *
 * Revision 1.4  2001/08/06 12:16:24  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.3.4.1  2001/08/03 13:44:01  peter
 * pragma once obsolete...
 *
 * Revision 1.3  2001/07/06 01:47:22  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.2  2001/01/07 13:05:25  peter
 * *** empty log message ***
 *
 */

