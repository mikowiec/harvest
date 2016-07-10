/* $Id: snd_openal.cpp,v 1.5 2003/06/04 17:14:48 pstrand Exp $ */

#include "os/compat.h"

#include <string>

#include "os/exceptions.h"
#include "os/debug.h"
#include "os/snd.h"
#include "os/snd/snd_types.h"
#include "os/snd/snd_subsystem.h"
#include "os/interfaces.h"
#include "misc/stlhelper.h"
#include "main/types.h"
#include "misc/resmgr.h"
#include "main/types_io.h"
#include "misc/plugin.h"

#include "AL/al.h"
#include "AL/alc.h"
#include "AL/altypes.h"
#include "AL/alut.h"


#ifndef WIN32
typedef void ALCcontext;
#endif


void alSourcefv(int a, int b, const reaper::Vector& vec)
{
	alSource3f(a, b, vec.x, vec.y, vec.z);
}


template<class T>
void alListenerfv(ALenum param, const reaper::Vec<T>& vec)
{
	alListenerfv(param, const_cast<ALfloat*>(vec.get()));
}



namespace reaper
{
namespace os
{
namespace snd
{
namespace openal
{

typedef ALuint source_id;


ALenum al_chk_err(const char* s)
{
	ALenum err = alGetError();
	if (err != AL_NO_ERROR) {
		printf("ALError %s: %x\n", s, err);
		throw hw_error("(openal) arg!");
	}
	return err;
}

const int num_srcs = 40;


class SrcIdT {
	source_id ids[num_srcs];
	int i;
public:
	typedef source_id T;
	bool init()
	{
		return true;
	}
	void release(source_id& id)
	{
		alDeleteSources(1, &id);
	}
	bool make(source_id& id)
	{
		alGenSources(1, &id);
		return alGetError() == AL_NO_ERROR;
	}
};

typedef misc::ResourceMgr<SrcIdT, num_srcs> SrcMgr;

const Vector zero = Vector(0.0, 0.0, 0.0);

typedef ALuint buffer_id;

struct Buffer {
	buffer_id id;
	int use;
};

ALenum info2format(const SoundInfo& info)
{
	if (info.channels == 2)
		return info.bits_per_sample == 16 ? AL_FORMAT_STEREO16 : AL_FORMAT_STEREO8;
	else
		return info.bits_per_sample == 16 ? AL_FORMAT_MONO16 : AL_FORMAT_MONO8;
}

class WaveEffect : public Effect
{
	ifs::Snd* snd;
	SrcMgr& mgr;

	Buffer* buf;

	int src_ix;
	source_id src_id;

	Point pos;
	Vector vec, vel;
	float pitch, vol;
	bool loop;

	bool valid()
	{
		return src_ix != -1;
	}
	bool alloc()
	{
		src_ix = mgr.alloc();
		if (src_ix == -1)
			return false;
		src_id = mgr.get(src_ix);
		return true;
	}
	void release()
	{
		al_chk_err("stop begin");

		if (src_ix != -1) {
			mgr.release(src_ix);
			alSourcei(src_id, AL_BUFFER, 0);
		}
		src_ix = -1;

		al_chk_err("stop end");
	}
public:
	WaveEffect(ifs::Snd* s, SrcMgr& m, Buffer* b)
	 : snd(s), mgr(m), buf(b), src_ix(-1), src_id(0),
	   pos(0,0,0), vec(0,0,0), vel(0,0,0), 
	   pitch(1.0), vol(1.0), loop(false)
	{
		al_chk_err("WaveEffect begin");
		++buf->use;
		al_chk_err("WaveEffect end");
	}
	~WaveEffect()
	{
		al_chk_err("~WaveEffect begin");
		stop();
		if (--buf->use == 0) {
			alDeleteBuffers(1, &buf->id);
			al_chk_err("~WaveEffect 1");
			delete buf;
		}
		al_chk_err("~WaveEffect end");
	}
	void play()
	{
		al_chk_err("play begin");
		if (src_ix == -1) {
			if (!alloc()) {
				return;
			}

			al_chk_err("play 1");
			alSourcei(src_id, AL_BUFFER, buf->id);
			alSource3f(src_id, AL_POSITION, pos.x, pos.y, pos.z);
			alSource3f(src_id, AL_DIRECTION, vec.x, vec.y, vec.z);
			alSource3f(src_id, AL_VELOCITY, vel.x, vel.y, vel.z);
			al_chk_err("play 2");
			alSourcef(src_id, AL_PITCH, pitch);
			alSourcei(src_id, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
			alSourcef(src_id, AL_GAIN, vol);
			al_chk_err("play 3");

			alSourcei(src_id, AL_SOURCE_RELATIVE, AL_FALSE);

			al_chk_err("play 4");
			if (loop) {
				alSourcef(src_id, AL_MAX_DISTANCE, 600);
				alSourcef(src_id, AL_ROLLOFF_FACTOR, 0.6);
				alSourcef(src_id, AL_REFERENCE_DISTANCE, 20);
			} else {
				alSourcef(src_id, AL_MAX_DISTANCE, 600);
				alSourcef(src_id, AL_ROLLOFF_FACTOR, 0.2);
				alSourcef(src_id, AL_REFERENCE_DISTANCE, 200);
			}
		}


		al_chk_err("play 3");

//		snd->derr() << "play: " << src_id << ' ' << buf->id << ' '
//	    			<< pos << ' ' << vec << ' ' << vel << ' ' 
//    				<< pitch << ' ' << loop << ' ' << vol << '\n';
		alSourcePlay(src_id);

		al_chk_err("play end");
	}
	void stop()
	{
		al_chk_err("play begin");
		if (valid()) {
			alSourceStop(src_id);
		}
		release();
		al_chk_err("play end");
	}
	void set_position(const Point& p)
	{
		al_chk_err("pos begin");
		if (valid())
			alSource3f(src_id, AL_POSITION, pos.x, pos.y, pos.z);
		pos = p;
		al_chk_err("pos end");
	}

	void set_direction(const Vector& v)
	{
		al_chk_err("dir begin");
		if (valid())
			alSource3f(src_id, AL_DIRECTION, vec.x, vec.y, vec.z);
		vec = v;
		al_chk_err("dir end");
	}

	void set_velocity(const Vector& v)
	{
		al_chk_err("vel begin");
		if (valid()) {
			alSource3f(src_id, AL_VELOCITY, vel.x, vel.y, vel.z);
		}
		vel = v;
		al_chk_err("vel end");
	}
	void set_pitch(float p)
	{
		al_chk_err("pitch begin");
		if (valid())
			alSourcef(src_id, AL_PITCH, p);
		pitch = p;
		al_chk_err("pitch end");
	}
	void looping(bool l)
	{
		al_chk_err("loop begin");
		if (valid())
			alSourcei(src_id, AL_LOOPING, l);
		loop = l;
		al_chk_err("loop end");
	}
	bool is_done()
	{
		int state;
		alGetSourcei(src_id, AL_SOURCE_STATE, &state);
		return (state != AL_PLAYING);
	}
	void set_volume(float v)
	{
		al_chk_err("vol begin");
		if (valid())
			alSourcef(src_id, AL_GAIN, vol);
		vol = v;
		al_chk_err("vol end");
	}

	EffectPtr clone() const
	{
		return EffectPtr(new WaveEffect(snd, mgr, buf));
	}
};

/*
class MusicSound : public Sound, private Sound_impl
{
//	std::deque<buffer_id> buf_ids;
	AudioSourcePtr dec;
	SoundInfo info;
public:
	MusicSound(SrcMgr& m, AudioSourcePtr d)
	 : Sound_impl(m), dec(d), info(dec->info())
	{
	}
	~MusicSound()
	{
	}

	bool do_stuff()
	{
		buffer_id id;
		alGenBuffers(1, &id);
//		buf_ids.push_back(id);

		int size = 0;
		Samples frames;
		bool cont = dec->read(frames);
		alBufferData(id, info2format(info),
			     const_cast<char*>(&frames[0]), frames.size(), info.samplerate);
		al_chk_err("BufferData");
		alSourceQueueBuffers(src_id, 1, &id);
		al_chk_err("SourceQueueBuffers");

//		alSourceUnqueueBuffers(src_id,
		
		return cont;
	}

	void play()
	{
		do_stuff();
		Sound_impl::play();
	}
	void stop()
	{
	}
	void set_volume(float)
	{
	}

	void fade_in(float) { }
	void fade_out(float) { }
};
*/


class Subsystem
 : public snd::Subsystem
{
	ifs::Snd* snd;
	bool is_init;
	ALCcontext* cxt;
	ALCdevice* dev;
	SrcMgr mgr;
	float vol;
public:
	Subsystem(ifs::Snd* s)
	 : snd(s), is_init(false), cxt(0), vol(1.0)
	{
	}

	~Subsystem()
	{
		snd->derr() << "openal shutdown..\n";
		mgr.release_all();
		alcMakeContextCurrent(0);
		alcDestroyContext(cxt);
		alcCloseDevice(dev);
		snd->derr() << "done\n";
	}

	bool init();
	void set_listener(const Point& pos, const Vector& dir, const Vector& vel);
	void set_volume(float vol);

	EffectPtr prepare(AudioSourcePtr);
	SoundPtr prepare_streaming(AudioSourcePtr);
};

bool Subsystem::init()
{
	if (is_init)
		return true;
	char *argv[] = {"dummy", 0};
	int argc = 1;
#ifdef WIN32
	std::string cfg = snd->config("openal_context");
	if (cfg.empty())
		cfg = "DirectSound";
	unsigned char* p = (unsigned char*)cfg.c_str();
#else
	unsigned char* p = 0;
#endif
	dev = alcOpenDevice(p);
	if (!dev)
		return false;
	cxt = alcCreateContext(dev, 0);
	if (!cxt)
		return false;
	alcMakeContextCurrent(cxt);

	if (alcGetCurrentContext() == 0) {
		return false;
	}


	Vector listenerPos(0.0, 0.0, 0.0);
	Vector listenerVel(0.0, 0.0, 0.0);
	ALfloat	listenerOri[] = {0.0, 1.0, 0.0, 0.0, 0.0, 1.0};

	al_chk_err("init 1");
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	al_chk_err("init 3");
	alListenerf(AL_GAIN, 1.0);
	al_chk_err("init 4");

	// NONE | INVERSE_DISTANCE | INVERSE_DISTANCE_CLAMPED
//	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

	// 0 = off, 1.0 = default
//	alDopplerFactor(0);
	al_chk_err("init 5");

	// Reference velocity
//	alDopplerVelocity(1.0);
	al_chk_err("init 6");
	is_init = true;
	snd->derr() << "OpenAL sound init\n";
	if (!mgr.data.init())
		throw hw_error("OpenAL source initialization error!");
	mgr.init();
	return true;
}

EffectPtr Subsystem::prepare(AudioSourcePtr sd)
{
	al_chk_err("prep begin");

	Buffer*  buf = new Buffer;
	ALenum format = info2format(sd->info());

	alGenBuffers(1, &buf->id);
	buf->use = 0;
	Samples smp;
	sd->read(smp);
	alBufferData(buf->id, format, &smp[0], smp.size(), sd->info().samplerate);
	delete sd;

	al_chk_err("prep end");
	EffectPtr eff(new WaveEffect(snd, mgr, buf));
	eff->set_volume(vol);
	return eff;
}

SoundPtr Subsystem::prepare_streaming(AudioSourcePtr dec)
{
	return SoundPtr(0);
//	return SoundPtr(new MusicSound(mgr, dec));
}

void Subsystem::set_listener(const Point& pos, const Vector& dir, const Vector& vel)
{
	alListenerfv(AL_POSITION, pos);
	float ori[] = { 0, 1, 0, dir.x, dir.y, dir.z };
	alListenerfv(AL_ORIENTATION, ori);
//	alListenerfv(AL_VELOCITY, vel);
}



void Subsystem::set_volume(float v)
{
	vol = v;
}


}
}
}
}


extern "C" {
DLL_EXPORT void* create_snd_openal(reaper::os::ifs::Snd* d)
{
	return new reaper::os::snd::openal::Subsystem(d);
}
}



#ifdef REAPER_EMBED_PLUGINS
namespace {
struct RegisterPlugin {
	RegisterPlugin() {
		reaper::misc::preloaded::add_plugin_creator("snd_openal", (reaper::misc::VoidCreator1)create_snd_openal);
	}
} foo;
}
#endif
