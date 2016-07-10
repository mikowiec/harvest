/* $Id: snd_system.cpp,v 1.3 2003/05/05 08:53:15 pstrand Exp $ */


#include "os/compat.h"

#include <deque>
#include <map>
#include <functional>
#include <string>

#include "os/snd.h"
#include "os/snd/snd_wave.h"
#include "os/snd/snd_mp3.h"
#include "os/snd/snd_subsystem.h"
#include "os/osinfo.h"

#include "res/config.h"
#include "res/res.h"
#include "misc/free.h"
#include "misc/sequence.h"
#include "misc/plugin.h"
#include "os/interfaces.h"
#include "world/world.h"
#include "res/resource.h"

#include "os/debug.h"
#include "os/worker.h"

#ifdef MONOLITHIC
extern "C" reaper::os::snd::Subsystem* create_snd_dummy(void*);
#endif

namespace reaper {
namespace os {
namespace snd {

namespace { 
	debug::DebugOutput derr("snd_subsystem", 0);
	debug::DebugOutput dlog("snd_subsystem", 5);
}

using misc::seq;
using misc::for_each;
using misc::delete_it;

using std::string;

typedef misc::Plugin<Subsystem, void, misc::Cache> SndProxy;

class Main
 : public ifs::Snd
{
	debug::DebugOutput dr;
public:
	Main() : dr("snd_plugin", 5)
	{ }
	debug::DebugOutput& derr() { return dr; }
	std::string config(const std::string& s) {
		res::ConfigEnv env("hw_snd_system");
		return env[s];
	}
	void add_job(worker::Job* j) {
		worker::worker()->add_job(j, false);
	}
};

template<class T>
class LazyLoad
{
	typedef misc::Plugin<T, void> Plugin;
	std::auto_ptr<Plugin> plugin;

public:
	LazyLoad() : plugin(0) { }

	Plugin& get()
	{
		if (!plugin.get()) {
			plugin = std::auto_ptr<Plugin>(new Plugin());
		}
		return *plugin;
	}
};

using namespace res;


typedef LazyLoad<AudioDecoder> SndDec;

class AudioDecoderCreator : public NodeConfig<AudioDecoder>
{
	typedef tp<AudioDecoder>::ptr Ptr;

	SndDec snd_dec;
public:
	
	Ptr create(IdentRef id) {
		res::res_stream* rs = 0;
		AudioDecoder* dec = 0;

		try {
			rs = new res::res_stream(res::Sound, id);
			dec = snd_dec.get().create("snd_wave", (void*)0);
		} catch (res::resource_not_found) {
			try {
				rs = new res::res_stream(res::Music, id);
				dec = snd_dec.get().create("snd_mp3", (void*)0);
			} catch (res::resource_not_found) { }
		}
		if (dec) {
			dec->init(rs);
			return Ptr(dec);
		} else {
			delete rs;
			return Ptr(0);
		}

	}
};


struct SoundSystem_impl
{
	SndProxy snd_proxy;

	float glob_volume;
	float ratio;

	SndDec snd_dec;

	Subsystem* music_player;
	Subsystem* sound_player;
	Subsystem* dummy;

	std::map<string, AudioDecoder*> decoders;

	Main main;

	SoundSystem_impl()
	 : glob_volume(0.5), ratio(0.0),
	   music_player(0), sound_player(0),
	   dummy(0)
	{
		dlog << "SoundSystem_impl enter\n";
		res::ConfigEnv env("hw_snd_system");

		string pfx("snd_");

		dummy = snd_proxy.create(pfx + "dummy", &main);

		string music = res::withdefault(env, "music", "simple");
		string sound = res::withdefault(env, "sound", "openal");
		if ((os_name() != "win32") && (sound == "dsound"))
			sound = "openal";
		try {
			music_player = snd_proxy.create(pfx + music, &main);
			dlog << "using " << music << " for music\n";
		} catch (error_base& e) {
			derr << "failed to load " << music
			     << " for music, reason: " << e.what() << '\n';
			music_player = dummy;
		}
		try {
			sound_player = snd_proxy.create(pfx + sound, &main);
			dlog << "using " << sound << " for sound\n";
		} catch (error_base& e) {
			derr << "failed to load " << sound
			     << " for sound, reason: " << e.what() << '\n';
			sound_player = dummy;
		}

		sound_player->set_volume(res::withdefault(env, "sound_volume", 0.8));
		music_player->set_volume(res::withdefault(env, "music_volume", 0.3));
		
		dlog << "SoundSystem_impl done\n";

//		res::push_config<AudioDecoder>(new AudioDecoderCreator(), "audio");
	}

	~SoundSystem_impl()
	{
		dlog << "~SoundSystem_impl enter\n";
		shutdown();
/*
		for_each(seq(decoders), delete_it);

		if (dummy != sound_player && dummy != music_player)
			delete dummy;
		if (sound_player != music_player)
			delete sound_player;
		delete music_player;
*/
//		res::pop_config<AudioDecoder>();
		dlog << "~SoundSystem_impl done\n";
	}

	void shutdown()
	{
		for_each(seq(decoders), delete_it);

		if (dummy != sound_player && dummy != music_player)
			delete dummy;
		if (sound_player != music_player)
			delete sound_player;
		delete music_player;
		dummy = sound_player = music_player = 0;

	}

	AudioSourcePtr load(const string& id)
	{
		std::map<string, AudioDecoder*>::iterator i = decoders.find(id);
		if (i != decoders.end())
			return i->second->get();

		res::res_stream* rs = 0;
		AudioDecoder* dec = 0;
		try {
			rs = new res::res_stream(res::Sound, id, res::throw_on_error);
			dec = snd_dec.get().create("snd_wave", (void*)0);
		} catch (res::resource_not_found) {
			try {
				rs = new res::res_stream(res::Music, id, res::throw_on_error);
				dec = snd_dec.get().create("snd_mp3", (void*)0);
			} catch (res::resource_not_found) { }
		}
		if (dec) {
			dec->init(rs);
			decoders[id] = dec;
			return dec->get();
		} else {
			delete rs;
			return 0;
		}
	}
/*
	AudioSourcePtr load(const string& id)
	{
		return res::resource<AudioDecoder>("audio", id).get();
	}
*/
};


SoundSystem::SoundSystem()
 : impl(new SoundSystem_impl())
{
}

SoundSystem::~SoundSystem()
{
	delete impl;
}


bool SoundSystem::init()
{
	impl->sound_player->init();
	impl->music_player->init();
	return true;
}

EffectPtr SoundSystem::prepare_effect(const string& id)
{

	EffectPtr eff;
	try {
		if (AudioSource* src = impl->load(id))
			eff = impl->sound_player->prepare(src);
	} catch (error_base& h) {
		derr << "sound preparation error: " << h.what()
		     << " for " << id << '\n';
	}
	if (eff.get())
		return eff;
	else
		return dummysound();
}

SoundPtr SoundSystem::prepare_music(const string& id)
{
	try {
		if (AudioSource* src = impl->load(id))
			return impl->music_player->prepare_streaming(src);
	} catch (error_base& e) {
		derr << "failed to load music: " << e.what() << '\n';
		return SoundPtr(dummysound().release());
	}
	return SoundPtr(0);
}

EffectPtr SoundSystem::dummysound()
{
	return impl->dummy->prepare((AudioSourcePtr)0);
}

void SoundSystem::set_volume(float vol)
{
	if (impl->ratio < 0) {
		impl->sound_player->set_volume(vol * (1 + impl->ratio));
		impl->music_player->set_volume(vol);
	} else {
		impl->sound_player->set_volume(vol);
		impl->music_player->set_volume(vol * (1 - impl->ratio));
	}
	impl->glob_volume = vol;
}

void SoundSystem::set_listener(const Point& pos, const Vector& dir, const Vector& vel)
{
	impl->sound_player->set_listener(pos, dir, vel);
}


void SoundSystem::do_stuff()
{
	if (impl->music_player)
		impl->music_player->do_stuff();
}

void SoundSystem::shutdown()
{
	impl->shutdown();
}


}
}
}

