#if 0
#include <AL/al.h>
#include <AL/altypes.h>
#include <AL/alut.h>
#endif

#ifdef _WIN32
#include "hw/windows.h"
#else
#ifdef SOLARIS
#include <sys/audioio.h>
#else
#include <sys/soundcard.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif


#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <fstream>

#include "../mpegsound.h"


struct snd_settings {
	bool stereo;
	int speed;
	int bitspersample;
};

bool operator!=(const snd_settings& s1, const snd_settings& s2)
{
	return s1.stereo != s2.stereo
	    || s1.speed != s2.speed
	    || s1.bitspersample != s2.bitspersample;
}

class FileOut : public Soundplayer
{
	int channels;
	int freq;
	int bps;
	std::ofstream wav_out;
	WaveHeader wave_out;
	WaveHeader2 wave_out2;
	char* buf;
	int last;
public:
	bool initialize(char* fname) {
		wav_out.open(fname, std::ios::out | std::ios::binary);
		buf = new char[100000000];
		last = 0;
		return true;
	}
	bool setsoundtype(int stereo, int samplesize, int speed) {
	
		channels = stereo ? 2 : 1;
		bps = samplesize;
		freq = speed;
		return true;
	}
	~FileOut() {
		wave_out.sig_riff[0] = 'R';
		wave_out.sig_riff[1] = 'I';
		wave_out.sig_riff[2] = 'F';
		wave_out.sig_riff[3] = 'F';
		wave_out.size = last;
		wave_out.sig_wave[0] = 'W';
		wave_out.sig_wave[1] = 'A';
		wave_out.sig_wave[2] = 'V';
		wave_out.sig_wave[3] = 'E';
		wave_out.sig_fmt[0] = 'f';
		wave_out.sig_fmt[1] = 'm';
		wave_out.sig_fmt[2] = 't';
		wave_out.sig_fmt[3] = ' ';
		wave_out.fill0 = 16;
		wave_out.fill1 = 1;
		wave_out.channels = channels;
		wave_out.sample_rate = freq;
		wave_out.bytes_per_sec = freq * channels * bps/8;
		wave_out.block_align = 4;
		wave_out.bits_per_sample = bps;
		wave_out2.sig_data[0] = 'd';
		wave_out2.sig_data[1] = 'a';
		wave_out2.sig_data[2] = 't';
		wave_out2.sig_data[3] = 'a';
		wave_out2.num_samples = wave_out.size / 4;
		wave_out2.fill2 = 0;

		wav_out.write((char*)&wave_out, sizeof(wave_out));
		wav_out.write((char*)&wave_out2, sizeof(wave_out2) - 2);
		wav_out.write(buf, last);
	}
	bool putblock(void* buffer, int size) {
		memcpy(buf + last, buffer, size);
		last += size;
		return true;
	}
	int get_size() { return last; }
	char* get_buf() { return buf; }
};

class TestOut : public Soundplayer
{
	short int rawbuffer[2*2*32*18];
	int rawbuffersize;
	snd_settings settings;
#ifdef _WIN32
	HWAVEOUT snd_hnd;
	WAVEFORMATEX wavefmt;
	int nextblk;
	WAVEHDR waveblk[32];
#else
	int dsp_fd;
#endif
public:
	TestOut() { }
	~TestOut() {
#ifdef _WIN32
#else
		close(dsp_fd);
#endif
	}
	bool initialize();
	bool initialize(char*) { return initialize(); }
	bool setsoundtype(int stereo,int samplesize,int speed);
	bool putblock(void *buffer,int size);
	bool resetsoundtype();
};

#ifdef _WIN32
void waveerr(int res)
{
	switch (res) {
	case MMSYSERR_ALLOCATED: std::cout << "Specified resource is already allocated.\n"; break;
	case MMSYSERR_BADDEVICEID: std::cout << "Specified device identifier is out of range.\n"; break; 
	case MMSYSERR_NODRIVER: std::cout << "No device driver is present.\n"; break;
	case MMSYSERR_NOMEM: std::cout << "Unable to allocate or lock memory.\n"; break;
	case WAVERR_BADFORMAT: std::cout << "Attempted to open with an unsupported waveform-audio format.\n"; break;
	case WAVERR_SYNC: std::cout <<  "The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag.\n"; break;
	}
}
#endif

namespace {
	int free_blks;
}

bool TestOut::initialize()
{
	rawbuffersize = 0;
#ifdef _WIN32

	settings.stereo = true;
	settings.speed = 44100;
	settings.bitspersample = 16;
	resetsoundtype();
#else
#ifdef SOLARIS
	dsp_fd = open("/dev/audio", O_WRONLY, 0);
#else
	dsp_fd = open("/dev/dsp", O_WRONLY, 0);
#endif
	if (dsp_fd == -1)
		return seterrorcode(SOUND_ERROR_DEVOPENFAIL);
#endif

	nextblk = 0;
	free_blks = 30;
	for (int i = 0; i < 32; i++) {
		memset(&waveblk[i], 0, sizeof(waveblk[0]));
		waveblk[i].lpData = (char*)malloc(10000);
		waveblk[i].dwBufferLength = 10000;
		
		waveOutPrepareHeader(snd_hnd, &waveblk[i], sizeof(waveblk[0]));
	}

	return true;
}

bool TestOut::setsoundtype(int stereo,int samplesize,int speed)
{
	snd_settings set;

	set.stereo = stereo == 1;
	set.bitspersample = samplesize;
	set.speed = speed;

	std::cout << "stereo: " << set.stereo
		  << " bps: " << set.bitspersample
		  << " speed: " << set.speed << '\n';
	if (set != settings) {
		settings = set;
		return resetsoundtype();
	}
	return true;
}

#ifdef _WIN32
void CALLBACK signal_fin(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,  
			 DWORD dwParam1, DWORD dwParam2)
{
	if (uMsg ==  WOM_DONE) {
		free_blks++;
	}

}
#endif


bool TestOut::resetsoundtype()
{
#ifdef _WIN32
	waveOutClose(snd_hnd);

	memset(&wavefmt, 0, sizeof(wavefmt));
	wavefmt.wFormatTag = WAVE_FORMAT_PCM;
	wavefmt.wBitsPerSample = settings.bitspersample;
	wavefmt.nSamplesPerSec = settings.speed;
	wavefmt.nChannels = settings.stereo ? 2 : 1;
	wavefmt.nBlockAlign = wavefmt.nChannels * wavefmt.wBitsPerSample / 8;
	wavefmt.nAvgBytesPerSec = settings.speed * wavefmt.nBlockAlign;
	int res = waveOutOpen(&snd_hnd, WAVE_MAPPER, &wavefmt, (DWORD)signal_fin, 0, CALLBACK_FUNCTION);

	if (res != MMSYSERR_NOERROR) {
		waveerr(res);
		return seterrorcode(SOUND_ERROR_DEVOPENFAIL);
	}

	{
		WAVEOUTCAPS caps;

		res= waveOutGetDevCaps((int)snd_hnd, &caps, sizeof(caps));
		if ( res != MMSYSERR_NOERROR ) {
			waveerr(res);
			return false;
		}
		std::cout << "Audio device: " << caps.szPname << '\n';
	}

//	int i;

#else
	int tmp;
//	ioctl(dsp_fd, SNDCTL_DSP_SYNC, 0);

#ifdef SOLARIS
	audio_info_t info;
	AUDIO_INITINFO(&info);
	info.play.encoding = AUDIO_ENCODING_LINEAR;
	info.play.precision = settings.bitspersample;
	info.play.sample_rate = settings.speed;
	info.play.channels = settings.stereo ? 2 : 1;
	if (ioctl(dsp_fd, AUDIO_SETINFO, &info) < 0)
		return seterrorcode(SOUND_ERROR_DEVCTRLERROR);
#else
	if (ioctl(dsp_fd, SNDCTL_DSP_STEREO, &settings.stereo) < 0)
		return seterrorcode(SOUND_ERROR_DEVCTRLERROR);

	if (ioctl(dsp_fd, SNDCTL_DSP_SETFMT, &settings.bitspersample) < 0)
		return seterrorcode(SOUND_ERROR_DEVCTRLERROR);

	if (ioctl(dsp_fd, SNDCTL_DSP_SPEED, &settings.speed) < 0)
		return seterrorcode(SOUND_ERROR_DEVCTRLERROR);
#endif
#endif
	return true;
}



bool TestOut::putblock(void *buffer,int sz)
{
#ifdef _WIN32
	int res;

	while (free_blks == 0) {
		Sleep(2);
	}
	if (sz > 10000) {
		std::cerr << "buffer to large\n";
		exit(1);
	}
	memcpy(waveblk[nextblk].lpData, buffer, sz);
	waveblk[nextblk].dwBufferLength = sz;

	res = waveOutWrite(snd_hnd, &waveblk[nextblk], sizeof(waveblk[0]));
	if (res != MMSYSERR_NOERROR) {
		waveerr(res);
		return seterrorcode(SOUND_ERROR_DEVOPENFAIL);
	}
	free_blks--;
	nextblk = (nextblk+1) % 32;


#else
	write(dsp_fd, buffer, sz);
#endif
	return true;
}
		

class TestIn : public Soundinputstream
{
	char* buf;
	char* p;
	long size;
public:
	TestIn()
	 : buf(0), p(0), size(0) { }
	~TestIn()
	{
		delete[] buf;
	}

	bool open(char *filename);
	bool _readbuffer(char *buffer,int bytes);
	int  getbytedirect(void);
	bool eof(void);
	int  getblock(char *buffer,int size);

	int  getsize(void);
	int  getposition(void);
	void setposition(int pos);
};



bool TestIn::open(char *filename)
{
	struct stat st;

	if (filename == 0) {
		seterrorcode(SOUND_ERROR_FILEOPENFAIL);
		return false;
	}

	stat(filename, &st);
	size = st.st_size;

	buf = new char[size];
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (! file.is_open()) {
		seterrorcode(SOUND_ERROR_FILEOPENFAIL);
		return false;
	}
	file.read(buf, size);
	if (! file.good()) {
		std::cout << "TestIn read error\n";
	}
	p = buf;
	return true;
}

int TestIn::getbytedirect()
{
	unsigned char c;
	if (p >= buf + size) {
		seterrorcode(SOUND_ERROR_FILEREADFAIL);
		return -1;
	}
	c = *p++;
	return c;
}

bool TestIn::_readbuffer(char *buffer, int sz)
{
	if (p + sz > buf + size) {
		seterrorcode(SOUND_ERROR_FILEREADFAIL);
		return false;
	}
	memcpy(buffer, p, sz);
	p += sz;
	return true;
}

bool TestIn::eof()
{
	return p >= buf + size;
};

int TestIn::getblock(char *buffer, int sz)
{
	memcpy(buffer, p, sz);
	p += sz;
	return sz;
}

int TestIn::getsize()
{
	return size;
}

void TestIn::setposition(int pos)
{
	p = buf + pos;
}

int  TestIn::getposition(void)
{
	return p - buf;
}


struct WaveData
{
	int bps;
	int channels;
	int size;
	int ssize;
	int freq;
	char* data;
};




int main(int argc, char* argv[])
{
//	alutInit(&argc, argv);


	{
		TestIn inp;
		inp.open("test.mp3");
		FileOut fout;
		fout.initialize("wintestout2.wav");

		Mpegtoraw mp3(&inp, &fout);

		mp3.initialize();

		std::cout << "version: " << mp3.getversion()
			  << " layer: " << mp3.getlayer()
			  << " frames: " << mp3.gettotalframe()
			  << " mode: " << mp3.getmode()
			  << " freq: " << mp3.getfrequency()
			  << " bitrate: " << mp3.getbitrate()
			  << '\n';

		mp3.run(-1);
		mp3.run(200);

	}
	{
		TestOut tout;
		tout.initialize();

		WaveData wd;
		std::ifstream wf("wintestout2.wav", std::ios::in | std::ios::binary);
		wf >> wd;
		long length = wd.size;
		tout.setsoundtype(1, 16, 44100);
		char* p = wd.data;
		while (p < wd.data + wd.size) {
			tout.putblock(p, 4000);
			p += 4000;
		}
	}
	{
		TestIn inp;
		inp.open("test.mp3");
		TestOut tout;
		tout.initialize();

		Mpegtoraw mp3(&inp, &tout);

		mp3.initialize();

		std::cout << "version: " << mp3.getversion()
			  << " layer: " << mp3.getlayer()
			  << " frames: " << mp3.gettotalframe()
			  << " mode: " << mp3.getmode()
			  << " freq: " << mp3.getfrequency()
			  << " bitrate: " << mp3.getbitrate()
			  << '\n';

		mp3.run(-1);
		mp3.run(200);
	}
	return 0;
}

