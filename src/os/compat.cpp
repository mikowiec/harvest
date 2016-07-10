
/*
 * $Author: pstrand $
 * $Date: 2002/12/29 16:58:14 $
 * $Log: compat.cpp,v $
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.12  2001/07/18 15:02:47  peter
 * no message
 *
 * Revision 1.11  2001/04/29 13:47:56  peter
 * windows...
 *
 * Revision 1.10  2001/01/30 00:03:49  peter
 * oops
 *
 * Revision 1.8  2001/01/05 01:36:51  peter
 * no message
 *
 */

#include "os/compat.h"

#ifdef WIN32

#ifndef REAPER_DLL_EXPORTS

#include "os/windows.h"
#include <stdio.h>

int main(int argc, char *argv[]);

int APIENTRY
WinMain(HINSTANCE, HINSTANCE, LPSTR p, int) {
	char *argv[100];
	argv[0] = "dumma windows!";
	int argc = 1;
	while (p && *p) {
		argv[argc++] = p;
		p = strchr(p, ' ');
		if (p)
			*p++ = 0;
	}
	argv[argc] = 0;
	return main(argc, argv);
}

#endif

#else


#endif


