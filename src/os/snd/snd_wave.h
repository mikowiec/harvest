/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:14 $
 * $Revision: 1.2 $
 */

#ifndef REAPER_HW_SND_WAVE
#define REAPER_HW_SND_WAVE

#include <vector>
#include <iosfwd>
#include "os/exceptions.h"

#include "os/snd/snd_types.h"

namespace reaper
{
namespace os
{
namespace snd
{


class wave_format_error_t : public hw_error_t { };


class WaveDecoder : public AudioDecoder
{
	SoundInfo wave_info;
	Samples smp;
public:
	WaveDecoder();
	~WaveDecoder();
	bool init(res::res_stream*);
	AudioSourcePtr get();
};


}
}
}

#endif

