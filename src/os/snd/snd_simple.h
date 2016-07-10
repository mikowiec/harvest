#ifndef REAPER_HW_SND_SIMPLE_H
#define REAPER_HW_SND_SIMPLE_H

#include "os/snd/snd_wave.h"


namespace reaper
{
namespace os
{
namespace snd
{
namespace simple
{


struct PlayerData;

class Player
{
	SoundInfo info;
	bool prepared;
	PlayerData* data;
public:
	Player();
	~Player();
	void prepare(const SoundInfo& info, int chunk_size, bool stream);
	void play(const Samples&);
	void set_volume(float level);
	void release();
};


}
}
}
}

#endif

/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:14 $
 * $Log: snd_simple.h,v $
 * Revision 1.2  2003/01/06 12:42:14  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.7  2002/01/21 00:20:57  peter
 * musikfixar, mm..
 *
 * Revision 1.6  2001/11/18 16:12:29  peter
 * hmm
 *
 * Revision 1.5  2001/08/06 12:16:23  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.4.4.1  2001/08/03 13:44:00  peter
 * pragma once obsolete...
 *
 * Revision 1.4  2001/07/06 01:47:21  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.3  2001/04/10 10:42:14  peter
 * omorg. av ljud...
 *
 * Revision 1.2  2001/03/15 03:21:59  peter
 * win32 fixar...
 *
 * Revision 1.1  2001/03/15 00:13:06  peter
 * wav&mp3
 *
 */

