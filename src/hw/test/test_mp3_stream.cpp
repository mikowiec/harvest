

/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:06 $
 * $Log: test_mp3_stream.cpp,v $
 * Revision 1.4  2003/01/06 12:42:06  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.3  2002/12/29 16:59:27  pstrand
 * hw/ moved to os/
 *
 * Revision 1.2  2001/08/20 16:59:39  peter
 * *** empty log message ***
 *
 * Revision 1.1  2001/04/21 16:47:33  peter
 * *** empty log message ***
 *
 * Revision 1.5  2001/04/10 20:48:19  peter
 * ljudfix
 *
 * Revision 1.4  2001/04/09 22:33:46  peter
 * ...
 *
 * Revision 1.3  2001/04/05 10:14:53  peter
 * picon vill ha mat, så det blir inga kommentarer... ;)
 *
 * Revision 1.2  2001/03/21 11:22:29  peter
 * *** empty log message ***
 *
 * Revision 1.1  2001/03/15 00:13:07  peter
 * wav&mp3
 *
 */

#include "os/compat.h"

#include "os/debug.h"
#include "os/snd_mp3.h"
#include "os/snd_simple.h"
#include "misc/plugin.h"
#include "net/sockstream.h"
#include "res/res.h"
#include "misc/test_main.h"

#include <iostream>

using namespace reaper;
using namespace reaper::os::snd;


int test_main()
{
	simple::Player player;
	misc::PluginCreator<SoundDecoder, misc::UniqueObj> snd_p;
	SoundDecoder* snd_dec = snd_p.create("snd_mp3");

	SoundInfo snd;
	net::sock_stream ss("64.152.82.194", 8826);
	ss << "GET /\r\n\r\n" << std::flush;
	char junk[1000];
	ss.getline(junk, 1000);

	snd_dec->init(&ss);

	player.prepare(snd_dec->info());
	Samples smp;
	while (!exit_now() && snd_dec->read(smp)) {
		player.play(smp);
	}
	return 0;
}

