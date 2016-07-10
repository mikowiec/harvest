/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:06 $
 * $Log: snd_test.cpp,v $
 * Revision 1.3  2003/01/06 12:42:06  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.2  2002/12/29 16:59:27  pstrand
 * hw/ moved to os/
 *
 * Revision 1.1  2001/01/06 05:46:42  peter
 * no message
 *
 */

#include "os/compat.h"

#include "os/gfx.h"
#include "os/snd.h"
#include "os/debug.h"

#include <iostream>

using reaper::os::gfx::Gfx;
using reaper::os::snd::SoundSystem;

int main()
{
	Gfx gfx;
	SoundSystem snd(gfx);

	return 0;
}
