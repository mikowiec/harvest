#ifndef HW_SND_H
#define HW_SND_H

#include "main/types.h"
#include <memory>

namespace reaper {
namespace os {
namespace snd {

/// Base for sounds. General settings.
class Sound
{
public:
	virtual ~Sound() { }

	/// Starts playing, from the beginning.
	virtual void play() = 0;

	/// Stops playing. Can be restarted again.
	virtual void stop() = 0;

	virtual void set_volume(float vol) = 0;

	virtual bool is_done() = 0;
};

class Effect;
typedef std::auto_ptr<Effect> EffectPtr;
typedef std::auto_ptr<Sound> SoundPtr;

/// 3d sound effect.
class Effect
 : public Sound
{
public:
	virtual void set_position(const Point&) = 0;
	virtual void set_direction(const Vector&) = 0;
	virtual void set_velocity(const Vector&) = 0;

	/// Ratio, 1.0 is normal rate.
	virtual void set_pitch(float p) = 0;

	/// Cheap cloning, should share resources if possible.
	virtual EffectPtr clone() const = 0;

	virtual void looping(bool) = 0;
};


struct SoundSystem_impl;

/** SoundSystem. Initializes sound system and loads sound/music files. */
class SoundSystem
{
	SoundSystem_impl* impl;
public:
	SoundSystem();
	~SoundSystem();

	bool init();

	/// Load and prepare sound effect.
	EffectPtr prepare_effect(const std::string& id);

	/// Load and prepare music (streamed sound).
	SoundPtr prepare_music(const std::string& id);

	/** Set global volume.
	 *  \param vol Volume (normalized 0..1)
	 */
	void set_volume(float vol);

	/// Set listener (camera) position/direction/velocity.
	void set_listener(const Point& pos, const Vector& dir, const Vector& vel);

	void do_stuff();

	/// Dummy implementation of effects, 
	std::auto_ptr<Effect> dummysound();

	void shutdown();
};


}
}
}

#endif


/*
 * $Author: pstrand $
 * $Date: 2003/05/05 08:52:25 $
 * $Log: snd.h,v $
 * Revision 1.3  2003/05/05 08:52:25  pstrand
 * sound changes..
 *
 * Revision 1.2  2003/01/06 12:42:12  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.38  2002/09/29 12:35:52  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.37  2002/09/27 18:05:04  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.36  2002/05/21 10:09:28  pstrand
 * *** empty log message ***
 *
 * Revision 1.35  2002/01/23 00:59:56  peter
 * soundfixes again
 *
 * Revision 1.34  2002/01/22 23:44:04  peter
 * reversed last two revs
 *
 * Revision 1.32  2002/01/17 04:58:45  peter
 * ljud-plugin-mm fix..
 *
 * Revision 1.31  2002/01/11 21:26:36  peter
 * audiosource, mm
 *
 * Revision 1.30  2002/01/07 14:00:27  peter
 * resurs och ljudmeck
 *
 * Revision 1.29  2001/11/26 02:20:08  peter
 * ljudfixar (smartptr), andra mindre fixar..
 *
 * Revision 1.28  2001/08/06 12:16:23  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.27.2.1  2001/08/03 13:43:59  peter
 * pragma once obsolete...
 *
 * Revision 1.27  2001/07/30 23:43:21  macke
 * Häpp, då var det kört.
 *
 * Revision 1.26  2001/07/27 15:47:35  peter
 * massive reorg...
 *
 * Revision 1.25  2001/07/06 01:47:21  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.24  2001/05/30 01:02:11  peter
 * ljudet funkar (bra) igen, för mig i alla fall =)
 *
 * Revision 1.23  2001/05/19 10:31:27  peter
 * ljudomorg..
 *
 * Revision 1.22  2001/05/03 11:03:20  peter
 * *** empty log message ***
 *
 */


