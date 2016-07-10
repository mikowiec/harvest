
/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:06 $
 * $Log: test_playmp3.cpp,v $
 * Revision 1.10  2003/01/06 12:42:06  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.9  2002/12/29 16:59:27  pstrand
 * hw/ moved to os/
 *
 * Revision 1.8  2001/08/20 16:59:39  peter
 * *** empty log message ***
 *
 * Revision 1.7  2001/07/15 21:42:46  peter
 * dynload, time fixar mm
 *
 *
 */

#include "os/compat.h"

#include "os/debug.h"
#include "os/snd_mp3.h"
#include "os/snd_simple.h"
#include "misc/plugin.h"
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
	res::res_stream snd_in(res::Music, "night");

	snd_dec->init(&snd_in);

	player.prepare(snd_dec->info());
	Samples smp;
	while (!exit_now() && snd_dec->read(smp)) {
		player.play(smp);
	}
	return 0;
}

