/*
 * $Author: fizzgig $
 * $Date: 2003/04/02 23:39:03 $
 * $Log: snd_simple_impl.cpp,v $
 * Revision 1.4  2003/04/02 23:39:03  fizzgig
 * added check and initialization for Playerdata.snd_hnd
 *
 * Revision 1.3  2003/03/19 22:09:54  pstrand
 * support embedded plugins
 *
 * Revision 1.2  2003/01/06 12:42:14  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.26  2002/06/18 12:20:43  pstrand
 * *** empty log message ***
 *
 * Revision 1.25  2002/03/11 10:50:48  pstrand
 * hw/time.h -> reltime.h&abstime.h, and some more..
 *
 * Revision 1.24  2002/02/05 16:41:43  peter
 * freebsd
 *
 * Revision 1.23  2002/02/02 14:05:07  peter
 * solaris
 *
 * Revision 1.22  2002/01/23 00:59:58  peter
 * soundfixes again
 *
 * Revision 1.21  2002/01/22 23:44:05  peter
 * reversed last two revs
 *
 * Revision 1.19  2002/01/21 09:32:59  peter
 * openal, mm.
 *
 * Revision 1.18  2002/01/21 00:20:58  peter
 * musikfixar, mm..
 *
 * Revision 1.17  2002/01/17 04:58:46  peter
 * ljud-plugin-mm fix..
 *
 * Revision 1.16  2002/01/07 16:38:26  peter
 * Singalong: "Vi hatar, vi hatar vc--, vi hatar de som vc skapat har..."
 *
 * Revision 1.15  2002/01/07 14:00:28  peter
 * resurs och ljudmeck
 *
 * Revision 1.14  2002/01/03 16:02:02  peter
 * no printf...
 *
 * Revision 1.13  2002/01/01 23:27:02  peter
 * musikfixar..
 *
 * Revision 1.12  2001/12/08 01:30:06  peter
 * no message
 *
 * Revision 1.11  2001/12/08 00:00:41  peter
 * mp3dekodning funkar (igen)...
 *
 * Revision 1.10  2001/11/26 02:20:09  peter
 * ljudfixar (smartptr), andra mindre fixar..
 *
 * Revision 1.9  2001/11/18 19:51:14  peter
 * asdf
 *
 * Revision 1.8  2001/11/18 16:12:29  peter
 * hmm
 *
 * Revision 1.7  2001/04/29 13:47:46  peter
 * windows...
 *
 * Revision 1.6  2001/04/20 15:27:45  peter
 * *** empty log message ***
 *
 * Revision 1.5  2001/04/18 01:01:40  peter
 * no message
 *
 * Revision 1.4  2001/04/18 00:45:17  peter
 * *** empty log message ***
 *
 * Revision 1.3  2001/04/16 22:14:22  peter
 * *** empty log message ***
 *
 * Revision 1.2  2001/04/10 20:48:19  peter
 * ljudfix
 *
 * Revision 1.1  2001/04/10 10:42:14  peter
 * omorg. av ljud...
 *
 */



#include "os/compat.h"

#include <string>

#include "os/snd/snd_simple.h"

#include <stdio.h>


#ifdef WIN32

#include "os/windows.h"

#pragma comment(lib, "winmm.lib")


const int wave_out_buffers = 8;

#else

#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef LINUX

#include <sys/soundcard.h>

#include "misc/resmgr.h"

const char sound_device[] = "/dev/dsp";

#endif


#ifdef FREEBSD

#include <sys/soundcard.h>
#include "misc/resmgr.h"

const char sound_device[] = "/dev/dsp";

#endif


#ifdef SOLARIS

#include <sys/audioio.h>
#include "misc/resmgr.h"

const char sound_device[] = "/dev/audio";

#endif


#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <string>
#include <iostream>
#include <fstream>
#include <map>

namespace reaper
{
namespace os
{
namespace snd
{
void put(std::ostream& os, const SoundInfo& si, int size);
void put(std::ostream& os, const Samples& s);
namespace simple
{


#if defined(LINUX) || defined(SOLARIS) || defined(FREEBSD)

class DSP
{
	int fd, stereo, bits, samplerate;
	bool open()
	{
		fd = ::open(sound_device, O_WRONLY);
		return fd >= 0;
	}
	bool configure(const SoundInfo& si)
	{
		stereo = (si.channels > 1);
		bits = si.bits_per_sample;
		samplerate = si.samplerate;
#ifdef FREEBSD
		return (ioctl(fd, SNDCTL_DSP_STEREO, &stereo) >= 0)
		    && (ioctl(fd, SNDCTL_DSP_SETFMT, &bits) >= 0)
		    && (ioctl(fd, SNDCTL_DSP_SPEED, &samplerate) >= 0);
#endif
#ifdef LINUX
		return (ioctl(fd, SNDCTL_DSP_STEREO, &stereo) >= 0)
		    && (ioctl(fd, SNDCTL_DSP_SETFMT, &bits) >= 0)
		    && (ioctl(fd, SNDCTL_DSP_SPEED, &samplerate) >= 0);
#endif
#ifdef SOLARIS
		audio_info_t ainfo;
		AUDIO_INITINFO(&ainfo);
		ainfo.play.encoding = AUDIO_ENCODING_LINEAR;
		ainfo.play.precision = bits;
		ainfo.play.sample_rate = samplerate;
		ainfo.play.channels = stereo + 1;
		return ioctl(fd, AUDIO_SETINFO, &ainfo) >= 0;
#endif
	}
public:
	DSP() : fd(-1) { }

	bool play(const SoundInfo& si, const Samples& s)
	{
		if (fd == -1 && !open())
			return false;
		if (si.channels != (stereo+1) ||
		    si.samplerate != samplerate ||
		    si.bits_per_sample != bits)
			configure(si);
		return write(fd, &s[0], s.size()) == s.size();
	}

	int get_fd() const
	{
		return fd;
	}
	bool operator==(const DSP& dsp) const
	{
		return fd == dsp.fd;
	}
	bool operator!=(const DSP& dsp) const
	{
		return !(*this == dsp);
	}
};

struct Res {
	typedef DSP T;

	// lazy initialization
	static bool make(const DSP& dsp)
	{ return true; }
};


typedef misc::ResourceMgr<Res, 20> DspMgr;
DspMgr dsp_mgr;

#endif


struct PlayerData
{
#ifdef WIN32
	HWAVEOUT snd_hnd;
	WAVEFORMATEX wavefmt;
	int free_bufs;
	int next_buf;
	int active_bufs;
	WAVEHDR wavebufs[wave_out_buffers];

        PlayerData() : snd_hnd(0) {}
#else
	int dsp;
#endif
};

#ifdef WIN32
std::string waveerr(int res)
{
	switch (res) {
	case MMSYSERR_ALLOCATED:
		throw hw_error("Specified resource is already allocated");
	case MMSYSERR_BADDEVICEID:
		throw hw_error("Specified device identifier is out of range");
	case MMSYSERR_NODRIVER:
		throw hw_error("No device driver is present");
	case MMSYSERR_NOMEM:
		throw hw_error("Unable to allocate or lock memory");
	case WAVERR_BADFORMAT:
		throw hw_error("Open unsupported waveform-audio format");
	case WAVERR_SYNC:
		throw hw_error("The device is synchronous");
	}
	throw hw_error("unknown error");
}
#endif

Player::Player()
 : prepared(false)
{
	info.channels = 2;
	info.samplerate = 44100;
	info.bits_per_sample = 16;
	data = new PlayerData;
}

Player::~Player()
{
#ifdef WIN32
	int i = 50;
	while (i-- > 0 && data->free_bufs < wave_out_buffers -1) {
		Sleep(10);
	}
	waveOutClose(data->snd_hnd);
#else
//	close(data->dsp_fd);
	if (data->dsp != -1)
		dsp_mgr.release(data->dsp);
#endif
	delete data;
}


#ifdef _WIN32
namespace {
	int* num_free_buffers;
}

void CALLBACK signal_fin(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,  
			 DWORD dwParam1, DWORD dwParam2)
{
	if (uMsg ==  WOM_DONE) {
		int* free = (int*)dwInstance;
		(*free)++;
//		printf("free: %d -> %d\n", (*free)-1, *free);
	}
}
#endif


void Player::prepare(const SoundInfo& new_info, int chunk_size, bool stream)
{
	info = new_info;
#ifdef WIN32


	memset(&data->wavefmt, 0, sizeof(WAVEFORMATEX));
	data->wavefmt.wFormatTag = WAVE_FORMAT_PCM;
	data->wavefmt.wBitsPerSample = info.bits_per_sample;
	data->wavefmt.nSamplesPerSec = info.samplerate;
	data->wavefmt.nChannels = info.channels;
	data->wavefmt.nBlockAlign = data->wavefmt.nChannels * data->wavefmt.wBitsPerSample / 8;
	data->wavefmt.nAvgBytesPerSec = info.samplerate * data->wavefmt.nBlockAlign;
	
        if(data->snd_hnd)         
            waveOutClose(data->snd_hnd);

	int res = waveOutOpen(&data->snd_hnd, WAVE_MAPPER, &data->wavefmt,
			(DWORD)signal_fin, (DWORD)&data->free_bufs, CALLBACK_FUNCTION);
	if (res != MMSYSERR_NOERROR)
		waveerr(res);

	data->next_buf = 0;
	num_free_buffers = &data->free_bufs;
	if (stream) {
		data->free_bufs = wave_out_buffers - 1;
		data->active_bufs = wave_out_buffers;
	} else {
		data->free_bufs = 1;
		data->active_bufs = 1;
	}
	for (int i = 0; i < wave_out_buffers; i++) {
		memset(&data->wavebufs[i], 0, sizeof(WAVEHDR));
		if (i < data->active_bufs) {
			data->wavebufs[i].lpData = (char*)malloc(chunk_size);
			data->wavebufs[i].dwBufferLength = chunk_size;
			int r = waveOutPrepareHeader(data->snd_hnd, &data->wavebufs[i], sizeof(WAVEHDR));
			if (r != MMSYSERR_NOERROR)
				printf("(simple)Player::prepare error!\n");
		} else {
			data->wavebufs[i].lpData = 0;
			data->wavebufs[i].dwBufferLength = 0;
		}
	}
//	printf("prepare: %d %d %d\n", info.bits_per_sample, info.stereo, info.samplerate);
#else
	if (!dsp_mgr.init())
		throw hw_error("dsp resmgr failed!");

	data->dsp = dsp_mgr.alloc();
#endif

	prepared = true;
}

void Player::set_volume(float v)
{
#ifdef WIN32
	DWORD s = v * 65535.0;
	//          left   right
	DWORD vol = s    + s*0x10000;
	int res = waveOutSetVolume(data->snd_hnd, vol);
	if (res != MMSYSERR_NOERROR)
		waveerr(res);
#endif
}

void Player::play(const Samples& samples)
{
	static int count = 0;
	if (! prepared)
		throw hw_error("Player: not prepared");
#ifdef WIN32
	int res;
	while (data->free_bufs == 0) {
		Sleep(2);
	}
	if (samples.size() != data->wavebufs[data->next_buf].dwBufferLength)
		throw hw_error("sample size mismatch");
	memcpy(data->wavebufs[data->next_buf].lpData, &samples[0], samples.size());

	res = waveOutWrite(data->snd_hnd, &data->wavebufs[data->next_buf], sizeof(WAVEHDR));
	count += data->wavebufs[data->next_buf].dwBufferLength;
	if (res != MMSYSERR_NOERROR)
		throw waveerr(res);
//	printf("play %d -> %d\n", data->free_bufs, data->free_bufs-1);
	data->free_bufs--;
	data->next_buf = (data->next_buf+1) % data->active_bufs;
#else
	if (data->dsp == -1) {
		data->dsp = dsp_mgr.alloc();
	}
	if (data->dsp == -1) {
//		printf("no dsp for me...\n");
		return;
	} else {
		dsp_mgr.get(data->dsp).play(info, samples);
	}
#endif
}

void Player::release()
{
#ifdef WIN32
	while (data->free_bufs < data->active_bufs-1)
		Sleep(2);
	for (int i = 0; i < data->active_bufs; i++) {
		waveOutUnprepareHeader(data->snd_hnd, &data->wavebufs[i], sizeof(WAVEHDR));
		free(data->wavebufs[i].lpData);
	}
	waveOutClose(data->snd_hnd);
#else
//	printf("rel: %d\n", data->dsp_fd);
	if (data->dsp != -1)
		dsp_mgr.release(data->dsp);
	data->dsp = -1;
#endif
}

}
}
}
}

