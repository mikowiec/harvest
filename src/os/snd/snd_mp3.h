/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:14 $
 * $Revision: 1.2 $
 */


#ifndef REAPER_HW_SND_MP3_H
#define REAPER_HW_SND_MP3_H

#include <iosfwd>

#include "os/snd/snd_types.h"

namespace reaper
{
namespace os
{
namespace snd
{

class Mp3Source;

class Mp3Decoder : public AudioDecoder
{
	res::res_stream* rs;
	Mp3Source* src;
public:
	Mp3Decoder();
	~Mp3Decoder();
	bool init(res::res_stream*);
	AudioSourcePtr get();
};


}
}
}

#endif

