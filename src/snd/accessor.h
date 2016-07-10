
/* 
 * Sound system accessor, to be embedded in objects and
 * used as interface for sound management.
 * 
 */


#ifndef REAPER_SND_ACCESSOR_H
#define REAPER_SND_ACCESSOR_H


namespace reaper {
namespace sound {

class Effect
{
public:
	virtual void set_position(const Point&) = 0;
	virtual void set_direction(const Vector&) = 0;
	virtual void set_velocity(const Vector&) = 0;

	virtual void set_pitch(float p) = 0;
	virtual void set_volume(float vol) = 0;

	virtual void play() = 0;
	virtual void stop() = 0;

	virtual ~Effect() { }
};

class Accessor
{
public:
	virtual Effect* make(const std::string& id, bool looping) = 0;
};


Accessor* mk_accessor();


}
}

#endif

