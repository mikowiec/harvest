/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:06 $
 * $Log: test_decodemp3.cpp,v $
 * Revision 1.4  2003/01/06 12:42:06  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.3  2002/12/29 16:59:27  pstrand
 * hw/ moved to os/
 *
 * Revision 1.2  2002/04/29 21:19:00  pstrand
 * *** empty log message ***
 *
 * Revision 1.1  2002/01/01 23:51:54  peter
 * ..
 *
 *
 */

#include "os/compat.h"

#include <iostream>

#include "os/snd_types.h"
#include "misc/test_main.h"
#include "misc/plugin.h"
#include "res/res.h"
#include "os/snd_wave.cpp"

namespace reaper {
namespace hw {
namespace snd {
void put(std::ostream& os, const SoundInfo& si, int size);
void put(std::ostream& os, const Samples& si);
}
}
}

using namespace reaper;
using namespace reaper::os::snd;
typedef misc::Plugin<AudioDecoder, void*> SndDecPlugin;

int test_main()
{
	
	SndDecPlugin snd_load;

	AudioDecoder* mp3_dec = snd_load.create("snd_mp3", 0);
	res::res_stream inp(res::Music, "reaper");
	res::res_out_stream out(res::Sound, "test_decode");

	mp3_dec->init(&inp);
	AudioSource* src = mp3_dec->get();
	put(out, src->info(), 10000);

	Samples smp;
	while (src->read(smp)) {
		put(out, smp);
	}
	
	return 0;
}

