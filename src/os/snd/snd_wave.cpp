
#include "os/compat.h"

#include <iostream>
#include <string>

#include "os/exceptions.h"
#include "os/bits.h"
#include "os/snd/snd_wave.h"
#include "misc/sequence.h"
#include "misc/plugin.h"

namespace reaper
{
namespace os
{
namespace snd
{

using lowlevel::uint16;
using lowlevel::uint32;
using lowlevel::to_le;

typedef error_tmpl<wave_format_error_t> wave_format_error;

/*

 wave file format:

 offset size  description
   0      4   "RIFF"
   4      4   size of rest of file
   8      4   "WAVE"
  12      4   "fmt "
  16      4   format size (16 or 18...)
  20      2   formattag (1 = PCM)
  22      2   channels (1 or 2)
  24      4   samples per second
  28      4   average bytes per second (rate * channels * bytes per sample)
  32      2   block align (channels * bytes per sample)
  34      2   bits per sample
  36 (38) 4   "data"
  40 (42) 4   size of data (in bytes)
  44 (46) ..  data
*/

struct WaveHeader
{
	char	sig_riff[4];
	uint32	size;
	char	sig_wave[4];
	char	sig_fmt[4];
	uint32	fmt_size;
	uint16	fmt_tag;
	uint16	channels;
	uint32	samplerate;
	uint32	bytes_per_sec;
	uint16	block_align;
	uint16	bits_per_sample;
	char	sig_data[4];
	uint32	data_size;
};

void put(std::ostream& os, const SoundInfo& si, int size)
{
	WaveHeader wave;
	wave.sig_riff[0] = 'R';
	wave.sig_riff[1] = 'I';
	wave.sig_riff[2] = 'F';
	wave.sig_riff[3] = 'F';
	wave.size = size + 44 - 8;
	wave.sig_wave[0] = 'W';
	wave.sig_wave[1] = 'A';
	wave.sig_wave[2] = 'V';
	wave.sig_wave[3] = 'E';
	wave.sig_fmt[0] = 'f';
	wave.sig_fmt[1] = 'm';
	wave.sig_fmt[2] = 't';
	wave.sig_fmt[3] = ' ';
	wave.fmt_size = 16;
	wave.fmt_tag = 1;
	wave.channels = si.channels;
	wave.samplerate = si.samplerate;
	wave.block_align = wave.channels * si.bits_per_sample / 8;
	wave.bytes_per_sec = wave.block_align * si.samplerate;
	wave.bits_per_sample = si.bits_per_sample;
	wave.sig_data[0] = 'd';
	wave.sig_data[1] = 'a';
	wave.sig_data[2] = 't';
	wave.sig_data[3] = 'a';
	wave.data_size = size;

	to_le(wave.size);
	to_le(wave.fmt_size);
	to_le(wave.fmt_tag);
	to_le(wave.channels);
	to_le(wave.samplerate);
	to_le(wave.bytes_per_sec);
	to_le(wave.block_align);
	to_le(wave.bits_per_sample);
	to_le(wave.data_size);

	os.write(reinterpret_cast<char*>(&wave), 44);
}

void put(std::ostream& os, const Samples& s)
{
	os.write(&s[0], s.size());
}

std::ostream& operator<<(std::ostream& os, SoundData& sd)
{
	put(os, sd.info, sd.data.size());
	put(os, sd.data);
	return os;
}

void get(std::istream& is, SoundInfo& info, int& size)
{
	using lowlevel::to_le;
	WaveHeader header;
	char* wavebuf = reinterpret_cast<char*>(&header);

	is.read(wavebuf, 36);
	if (! is.good())
		throw wave_format_error("read error\n");

	if (strncmp(header.sig_riff, "RIFF", 4))
		throw wave_format_error("no RIFF signature\n");
	if (strncmp(header.sig_wave, "WAVEfmt ", 8))
		throw wave_format_error("no WAVE or format signature\n");

	to_le(header.size);
	to_le(header.fmt_size);
	to_le(header.fmt_tag);
	to_le(header.channels);
	to_le(header.samplerate);
	to_le(header.bytes_per_sec);
	to_le(header.block_align);
	to_le(header.bits_per_sample);

	if (header.size < 44 || header.size > 50e6)
		throw wave_format_error("total size error\n");
	if (header.fmt_tag != 1)
		throw wave_format_error("unexpected type\n");
	if (header.channels != 1 && header.channels != 2)
		throw wave_format_error("channels error\n");
	if (header.samplerate < 1000 || header.samplerate > 1e6)
		throw wave_format_error("sample rate error\n");
	if (header.bits_per_sample != 8 && header.bits_per_sample != 16)
		throw wave_format_error("unexpected sample size");

	if (header.fmt_size > 16) {
		int ext = header.fmt_size - 16;
		if (ext > 20)
			throw wave_format_error("format chunk size error");
		char dump[20];
		is.read(dump, ext);
	}
	is.read(&wavebuf[36], 8);

	to_le(header.data_size);

	if (strncmp(header.sig_data, "data", 4))
		throw wave_format_error("no data signature\n");
	if (header.data_size == 0 || header.data_size > 50e6)
		throw wave_format_error("data size error\n");

	info.channels = header.channels;
	info.samplerate = header.samplerate;
	info.bits_per_sample = header.bits_per_sample;
	size = header.data_size;
}

void get(std::istream& is, Samples& s, int len)
{
	s.resize(len);
	is.read(&s[0], len);
}

std::istream& operator>>(std::istream& is, SoundData& sd)
{
	int size;
	get(is, sd.info, size);
	get(is, sd.data, size);

	if (! is.good())
		std::cerr << "data read error\n";
	return is;
}

class WaveSource : public AudioSource
{
	SoundInfo wave_info;
	Samples* smp;
	WaveSource(const WaveSource&);
public:
	WaveSource(const SoundInfo& wi, Samples* s);
	SoundInfo info();
	bool read(Samples&);
};


WaveSource::WaveSource(const SoundInfo& wi, Samples* s)
 : wave_info(wi), smp(s)
{
}

SoundInfo WaveSource::info()
{
	return wave_info;
}

bool WaveSource::read(Samples& s)
{
	s.resize(smp->size());
	memcpy(&s[0], &(*smp)[0], smp->size()); // visual doesn't like copy
	return false;
}

WaveDecoder::WaveDecoder()
 : smp(0)
{
}

WaveDecoder::~WaveDecoder()
{
}

bool WaveDecoder::init(res::res_stream* is)
{
	int size;
	snd::get(*is, wave_info, size);
	snd::get(*is, smp, size);
	delete is;
	return true;
}

AudioSourcePtr WaveDecoder::get()
{
	return new WaveSource(wave_info, &smp);
}


}
}
}

extern "C" {
DLL_EXPORT void* create_snd_wave(void*)
{
	return new reaper::os::snd::WaveDecoder();
}
}



#ifdef REAPER_EMBED_PLUGINS
namespace {
struct RegisterPlugin {
	RegisterPlugin() {
		reaper::misc::preloaded::add_plugin_creator("snd_wave", (reaper::misc::VoidCreator1)create_snd_wave);
	}
} foo;
}
#endif

