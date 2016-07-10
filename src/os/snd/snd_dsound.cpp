/* $Id: snd_dsound.cpp,v 1.7 2003/06/04 18:55:46 pstrand Exp $ */

#include "os/compat.h"


#include <string>
#include <vector>
#include <map>
#include <deque>
#include <algorithm>

#include "os/exceptions.h"
#include "os/debug.h"


#include "os/snd.h"
#include "os/snd/snd_simple.h"
#include "os/snd/snd_wave.h"
#include "os/snd/snd_mp3.h"
#include "os/snd/snd_subsystem.h"
#include "os/interfaces.h"

#include "os/win32_errors.h"

#include "os/worker.h"

#include "main/types_io.h"
#include "main/types_ops.h"
#include "misc/stlhelper.h"
#include "misc/smartptr.h"
#include "res/res.h"
#include "res/config.h"
#include "misc/plugin.h"

#include <dsound.h>


#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")



namespace reaper
{
namespace os
{

namespace snd
{
namespace dsound
{


struct UglyData
{
	bool is_init;
	LPDIRECTSOUND ds;
	DSCAPS caps;

	LPDIRECTSOUNDBUFFER prim_buffer;
	LPDIRECTSOUND3DLISTENER listener;

	ifs::Debug* dbg;
	UglyData(ifs::Debug* d)
	 : is_init(false), ds(0), prim_buffer(0), listener(0), dbg(d)
	{ }
};


class Subsystem
 : public snd::Subsystem
{
	UglyData data;
public:
	Subsystem(ifs::Debug* d);
	~Subsystem();
	bool init();
	void set_listener(const Point& pos, const Vector& dir, const Vector& vel);
	void set_volume(float vol);

	EffectPtr prepare(AudioSourcePtr);
	SoundPtr prepare_streaming(AudioSourcePtr);
};


Subsystem::Subsystem(ifs::Debug* d)
 : data(d)
{
	data.dbg->derr() << "Subsystem create\n";
}

Subsystem::~Subsystem()
{
	data.dbg->derr() << "Subsystem delete\n";
	if (data.listener)
		data.listener->Release();
	data.listener = 0;
	if (data.ds)
		data.ds->Release();
	data.ds = 0;
}

struct dsound_error : public hw_error {
	dsound_error(const std::string& s, int e)
	 : hw_error(s + ": " + ds_error(e)) { }
};

void dserror(const std::string& s, int e) {
	throw dsound_error(s, e);
}




void initformat(WAVEFORMATEX& format, const SoundInfo& si)
{
	memset(&format, 0, sizeof(WAVEFORMATEX));
	format.wFormatTag = WAVE_FORMAT_PCM;
	format.nChannels = si.channels;
	format.nSamplesPerSec = si.samplerate;
	format.wBitsPerSample = si.bits_per_sample;
	format.nBlockAlign = format.nChannels * (format.wBitsPerSample / 8);
	format.nAvgBytesPerSec = format.nBlockAlign * format.nSamplesPerSec;
	format.cbSize = 0;
}

void initbufdesc(DSBUFFERDESC& bufdesc, WAVEFORMATEX& format, long size, long flags = 0)
{
	memset(&bufdesc, 0, sizeof(DSBUFFERDESC));
	bufdesc.dwSize = sizeof(DSBUFFERDESC);
	bufdesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY  | DSBCAPS_LOCDEFER | flags;
	bufdesc.dwBufferBytes = size;
	bufdesc.lpwfxFormat = &format;
}


bool Subsystem::init()
{
	data.dbg->derr() << "Subsystem init: " << data.is_init << '\n';
	if (data.is_init)
		return true;

	int err;
	
	if ((err = DirectSoundCreate(0, &data.ds, 0)) != DS_OK)
		dserror("DirectSoundCreate", err);

	LPDIRECTSOUND ds = data.ds;


	if ((err = ds->SetCooperativeLevel(GetForegroundWindow(), DSSCL_NORMAL)) != DS_OK)
		dserror("SetCooperativeLevel", err);

	memset(&data.caps, 0, sizeof(DSCAPS));
	data.caps.dwSize = sizeof(DSCAPS);

	if ((err = ds->GetCaps(&data.caps)) != DS_OK)
		dserror("GetCaps", err);


	LPDIRECTSOUNDBUFFER prim_buffer;
	LPDIRECTSOUND3DLISTENER listener;

	DSBUFFERDESC bufdesc;
	memset(&bufdesc, 0, sizeof(bufdesc));
	bufdesc.dwSize = sizeof(bufdesc);
	bufdesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;

	if ((err = ds->CreateSoundBuffer(&bufdesc, &prim_buffer, 0)) != DS_OK)
		dserror("CreateSoundBuffer (3d)", err);

	if (prim_buffer->QueryInterface(IID_IDirectSound3DListener, reinterpret_cast<void**>(&listener)) == DS_OK) {
		((listener->SetPosition(0, 0, 0, DS3D_IMMEDIATE) == DS_OK) &&
		 (listener->SetOrientation(0, 0, 1, 0, 1, 0, DS3D_IMMEDIATE) == DS_OK) &&
//		 (listener->SetDopplerFactor(DS3D_MINDOPPLERFACTOR, DS3D_IMMEDIATE) == DS_OK) &&
		 (listener->SetDistanceFactor(0.01, DS3D_IMMEDIATE) == DS_OK)) || (dserror("3d init", 0),1);
	}
	data.listener = listener;
	data.prim_buffer = prim_buffer;


	data.is_init = true;
	data.dbg->derr() << "DirectSound init\n";

	return true;
}



const Vector zero = Vector(0.0, 0.0, 0.0);

typedef LPDIRECTSOUNDBUFFER buf_ptr;
typedef LPDIRECTSOUND3DBUFFER buf_3d_ptr;

class WaveBuffer
{
	bool mk_clone;
	bool is_init;
	UglyData* data;

	buf_ptr sndbuf;
	buf_3d_ptr sndbuf_3d;

	DSBUFFERDESC bufdesc;
	WAVEFORMATEX format;
	AudioSourcePtr sd;
	bool is_3d;
	WaveBuffer(const WaveBuffer&);
public:
	WaveBuffer()
	 : mk_clone(true), is_init(false), data(0), sndbuf(0), sndbuf_3d(0), sd(0), is_3d(false)
	{ }
	WaveBuffer(UglyData* p, AudioSourcePtr sp, bool i3d = true)
	 : mk_clone(true), is_init(false), data(p), sndbuf(0), sndbuf_3d(0), sd(sp), is_3d(i3d)
	{
	}

	bool is_ready() const { return is_init; }

	void init()
	{
		initformat(format, sd->info());
		Samples smp;
		sd->read(smp);
		initbufdesc(bufdesc, format, smp.size(),
			    DSBCAPS_STATIC | (is_3d ? DSBCAPS_CTRL3D : 0));

		int err;
		if ((err = data->ds->CreateSoundBuffer(&bufdesc, &sndbuf, 0)) != DS_OK)
			dserror("CreateSoundBuffer (wave)", err);
		if (is_3d) {
			if ((err = sndbuf->QueryInterface(IID_IDirectSound3DBuffer,
							  reinterpret_cast<void**>(&sndbuf_3d))
			    ) != DS_OK)
				dserror("QueryInterface (3d)", err);
		}


		sndbuf->SetCurrentPosition(0);
		char* p1;
		char* p2;
		DWORD l1, l2;
		if ((err = sndbuf->Lock(0, smp.size(), (LPVOID*)&p1, &l1, (LPVOID*)&p2, &l2, 0)) != DS_OK)
			dserror("Lock", err);
		std::copy(&smp[0], &smp[l1], p1);
		if (p2)
			std::copy(&smp[l1], &smp[l2], p2);
		if ((err = sndbuf->Unlock(p1, l1, p2, l2)) != DS_OK)
			dserror("Unlock", err);
		is_init = true;
	}
	~WaveBuffer()
	{
		if (sndbuf_3d)
			sndbuf_3d->Release();
		if (sndbuf)
			sndbuf->Release();
	}
	const SoundInfo info() { return sd->info(); }
	buf_ptr get_buf() { return sndbuf; }
	buf_3d_ptr get_3d_buf() { return sndbuf_3d; }

	WaveBuffer* clone()
	{
		if (!is_init)
			init();
		if (mk_clone) {
			WaveBuffer* dup = new WaveBuffer();
			dup->data = data;
			dup->sd = sd;
			int err;
			if ((err = data->ds->DuplicateSoundBuffer(sndbuf, &dup->sndbuf)) != DS_OK) {
				dserror("DuplicateSoundBuffer failed", err);
				return 0;
			}
			if (is_3d) {
				err = dup->sndbuf->QueryInterface(IID_IDirectSound3DBuffer,
						reinterpret_cast<void**>(&dup->sndbuf_3d));
				dup->is_3d = true;
			}
			if (err != DS_OK)
				dserror("QueryInterface (3d)", err);

			dup->is_init = true;
			return dup;
		} else {
			return new WaveBuffer(data, sd, is_3d);
		}
	}

};


unsigned long fill_bufs(Samples& from, char* p1, unsigned long l1)
{
	long len = from.size();
	if (len < l1) {
		std::copy(from.begin(), from.end(), p1);
		from.clear();
		return len;
	} else {
		std::copy(from.begin(), from.begin() + l1, p1);
		std::copy(from.begin() + l1, from.end(), from.begin());
		from.resize(len - l1);
		return l1;
	}
}


unsigned long fill_bufs(const Samples& from, char* p1, unsigned long l1, Samples& overflow)
{
	long len = from.size();
	if (len < l1) {
		std::copy(from.begin(), from.end(), p1);
		return len;
	} else {
		std::copy(from.begin(), from.begin() + l1, p1);
		overflow.resize(len - l1);
		std::copy(from.begin() + l1, from.end(), overflow.begin());
		return l1;
	}
}

class StreamBuffer
{
	int chunk_size;
	UglyData* data;
	AudioSourcePtr dec;

	buf_ptr sndbuf;

	Samples overflow;
	DSBUFFERDESC bufdesc;
	WAVEFORMATEX format;
	DWORD write_ptr;
	int have_read, have_played;
public:
	StreamBuffer(UglyData* p, AudioSourcePtr d)
	 : chunk_size(1024*1024), data(p), dec(d), write_ptr(0), have_read(0), have_played(0)
	{
		initformat(format, dec->info());
		initbufdesc(bufdesc, format, chunk_size);
		int err;
		if ((err = data->ds->CreateSoundBuffer(&bufdesc, &sndbuf, 0)))
			dserror("CreateSoundBuffer (stream)", err);
		
		sndbuf->SetCurrentPosition(0);
		overflow.clear();
	}

	bool fill_data()
	{
		using std::min;

		int err;

		DWORD play, write;
		sndbuf->GetCurrentPosition(&play, &write);
		printf("pos: %x|%x|%x %d     ", play, write, write_ptr, (play - write_ptr) % chunk_size);
		if ((play - write_ptr) % chunk_size < chunk_size/4) {
			printf("waiting...\n");
			return true;
		}

		char* p1;
		char* p2;
		unsigned long l1, l2;

		if ((err = sndbuf->Lock(write_ptr, chunk_size / 4, (LPVOID*)&p1, &l1, (LPVOID*)&p2, &l2, 0)) != DS_OK) {
			if (err == DSERR_INVALIDPARAM) {
				printf("buffer full, waiting...\n");
				return true;
			}
			printf("ERR! %s\n", ds_error(err));
			dserror("Lock", err);
		}
//		printf("%x  %d  %x %d  %d  %d\n", p1, l1, p2, l2, have_read, have_played);
		int off = 0;

		while (off < l1) {
			printf("fill primarybuffer ");
			if (overflow.empty()) {
				Samples smp;
				bool cont = dec->read(smp);
				have_read += smp.size();
//				printf("no overflow, reading %d |%d| ", smp.size(), overflow.size());
				int n = fill_bufs(smp, p1 + off, l1 - off, overflow);
//				printf(" wrote %d, %d in stash (%d, %d) ", n, overflow.size(), l1, l1 - off);
				off += n;
			} else {
//				printf("with overflow, %d (%d) ", overflow.size(), l1 - off);
				int n = fill_bufs(overflow, p1 + off, l1 - off);
//				printf(" wrote %d (%d) ", n, overflow.size());
				off += n;
			}
//			putchar('\n');
		}
		
		while (p2 && off < l1+l2) {
			printf("fill secondary buffer ");
			if (overflow.empty()) {
				Samples smp;
				bool cont = dec->read(smp);
				have_read += smp.size();
//				printf("no overflow, reading %d ", smp.size());
				Samples buf2;
				dec->read(buf2);
				int n = fill_bufs(buf2, &p2[off - l1], l2, overflow);
//				printf(" wrote %d, %d in stash (%d) ", n, overflow.size(), l2);
				off += n;
			} else {
//				printf("with overflow, %d ", overflow.size());
				int n = fill_bufs(overflow, p2 + off - l1, l2);
//				printf(" wrote %d ", n);
				off += n;
			}
//			putchar('\n');
		}

		write_ptr += off;
		write_ptr %= chunk_size;
		have_played += off;

		if ((err = sndbuf->Unlock(p1, l1, p2, l2)) != DS_OK)
			dserror("Unlock", err);
		putchar('\n');
		return true;
	}
	buf_ptr get_buf() { return sndbuf; }

};

class WaveEffect : public Effect
{
	UglyData* data;
	WaveBuffer* buf;
	bool loop;
	int org_freq;
	bool playing;
	bool valid() {
		if (buf->get_buf() == 0)
			data->dbg->derr() << "not valid!\n";
		return buf->get_buf() != 0;
	}
public:
	WaveEffect(UglyData* p, WaveBuffer* b)
	 : data(p), buf(b), loop(false), org_freq(b->info().samplerate), playing(false)
	{
		if (!buf->is_ready())
			b->init();
	}
	~WaveEffect()
	{
		stop();
		delete buf;
	}

	void play()
	{
		if (!valid())
			return;
		if (!buf->is_ready())
			buf->init();
		data->dbg->derr() << "play: " << buf->get_buf() << ' ' << loop << '\n';
		buf->get_buf()->Stop();
		buf->get_buf()->Play(0, 0, DSBPLAY_TERMINATEBY_TIME | (loop ? DSBPLAY_LOOPING : 0));
		playing = true;
	}

	void stop()
	{
		if (!valid())
			return;
		data->dbg->derr() << "stop: " << buf->get_buf() << '\n';
		buf->get_buf()->Stop();
		playing = false;
	}

	void set_volume(float vol)
	{
		if (!valid())
			return;
		data->dbg->derr() << "vol: " << buf->get_buf() << "  " << vol << '\n';
		buf->get_buf()->SetVolume(DSBVOLUME_MIN + vol * abs(DSBVOLUME_MAX - DSBVOLUME_MIN));
	}

	void set_position(const Point& pos)
	{
		if (!valid())
			return;
//		data->dbg->derr() << "pos: " << buf->get_buf() << ' ' << playing << ' ' << pos << '\n';
/*
		if (rel_listener)
			buf->get_3d_buf()->SetMode(DS3DMODE_HEADRELATIVE, DS3D_DEFERRED);
		else
			buf->get_3d_buf()->SetMode(DS3DMODE_NORMAL , DS3D_DEFERRED);
*/
		buf->get_3d_buf()->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
//		buf->get_3d_buf()->SetPosition(0, 0, 0, DS3D_DEFERRED);
	}

	void set_direction(const Vector& vec)
	{
		if (!valid())
			return;
//		derr << "dir: " << buf->get_buf() << ' ' << vec << '\n';
//		buf->get_3d_buf()->SetConeOrientation(vec.x, vec.y, vec.z, DS3D_DEFERRED);
	}

	void set_velocity(const Vector& vel)
	{
		if (!valid())
			return;
//		data->dbg->derr() << "velocity: " << buf->get_buf() << ' ' << vel << '\n';
//		buf->get_3d_buf()->SetVelocity(vel.x, vel.y, vel.z, DS3D_DEFERRED);
	}
	void set_pitch(float p)
	{
		if (!valid())
			return;
//		data->dbg->derr() << "pitch: " << buf->get_buf() << ' ' << p << '\n';
//		buf->get_buf()->SetFrequency(org_freq * p);
	}
	void looping(bool l)
	{
		loop = l;
	}
	bool is_done()
	{
		if (!valid())
			return true;
		DWORD status;
		buf->get_buf()->GetStatus(&status);
		return status | DSBSTATUS_PLAYING;
	}
	EffectPtr clone() const
	{
		WaveBuffer* b = buf->clone();
		return EffectPtr(b == 0 ? 0 : new WaveEffect(data, b));
	}
};


class MusicSound
 : public Sound
{
	UglyData* data;
	StreamBuffer* buf;
	bool playing, exit;
	struct MusicJob : public worker::Job
	{
		MusicSound* we;
		MusicJob(MusicSound* w) : we(w) { }
		bool operator()()
		{
			return we->do_stuff();
		}
	};
public:
	MusicSound(UglyData* p, StreamBuffer* b)
	 : data(p), buf(b), playing(false), exit(false)
	{
	}
	~MusicSound()
	{
	}

	bool do_stuff()
	{
//		printf("do_stuff %d\n", playing ? 1 : 0);
		if (playing) {
			return buf->fill_data();
		} else {
			return exit;
		}
	}

	virtual void play()
	{
		printf("play\n");
		set_volume(1.0);
		buf->fill_data();
		playing = true;
//		worker::worker()->add_job(new MusicJob(this));
		buf->get_buf()->Play(0, 0, DSBPLAY_LOOPING);
	}

	virtual void stop()
	{
		printf("stop\n");
		buf->get_buf()->Stop();
		playing = false;
		exit = true;
	}

	void set_volume(float vol) {
		buf->get_buf()->SetVolume(DSBVOLUME_MIN + vol * abs(DSBVOLUME_MAX - DSBVOLUME_MIN));
	}
	void fade_in(float) { }
	void fade_out(float) { }
	bool is_done() { return true; }
};



EffectPtr Subsystem::prepare(AudioSourcePtr sd)
{
	WaveBuffer* buf = new WaveBuffer(&data, sd);
	buf->init();
	return EffectPtr(new WaveEffect(&data, buf));
}

SoundPtr Subsystem::prepare_streaming(AudioSourcePtr dec)
{
	StreamBuffer* buf = new StreamBuffer(&data, dec);
	return SoundPtr(new MusicSound(&data, buf));
}

void Subsystem::set_listener(const Point& pos, const Vector& dir, const Vector& vel)
{
	data.listener->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
	Vector front = norm(dir);
	data.listener->SetOrientation(front.x, front.y, front.z, 0, 1, 0, DS3D_DEFERRED);
//	data.listener->SetVelocity(vel.x, vel.y, vel.z, DS3D_DEFERRED);
	data.listener->CommitDeferredSettings();
}


void Subsystem::set_volume(float vol)
{
//	data.dbg->derr() << "Vol: " << vol << '\n';
}


}
}
}
}

extern "C" {
DLL_EXPORT void* create_snd_dsound(reaper::os::ifs::Debug* d)
{
	return new reaper::os::snd::dsound::Subsystem(d);
}
}


#ifdef REAPER_EMBED_PLUGINS
namespace {
struct RegisterPlugin {
	RegisterPlugin() {
		reaper::misc::preloaded::add_plugin_creator("snd_dsound", (reaper::misc::VoidCreator1)create_snd_dsound);
	}
} foo;
}
#endif

