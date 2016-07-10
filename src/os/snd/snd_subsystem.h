#ifndef REAPER_HW_SND_SUBSYSTEM_H
#define REAPER_HW_SND_SUBSYSTEM_H

#include <map>
#include "main/types.h"
#include "os/snd.h"

namespace reaper
{
namespace os
{
namespace snd
{

class Subsystem
{
public:
	virtual ~Subsystem() { }

	virtual bool init() = 0;
	virtual void set_listener(const Point& pos, const Vector& dir, const Vector& vel) = 0;
	virtual void set_volume(float vol) = 0;

	virtual EffectPtr prepare(AudioSourcePtr) = 0;
	virtual SoundPtr prepare_streaming(AudioSourcePtr) = 0;

	virtual void do_stuff() { }
};


}
}
}

#endif

