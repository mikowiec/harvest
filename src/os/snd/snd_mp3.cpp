/* $Id: snd_mp3.cpp,v 1.3 2003/03/19 22:09:53 pstrand Exp $ */

#include "os/compat.h"

#include "os/exceptions.h"
#include "os/snd/snd_mp3.h"

#include "ext/mpegsound/mpegsound.h"

#include <iostream>
#include "misc/iostream_helper.h"
#include "misc/plugin.h"

namespace reaper
{
namespace os
{
namespace snd
{


class StreamInput
 : public mpegsound::Soundinputstream
{
	std::istream* input;
	int size;
public:
	StreamInput(std::istream* is)
	 : input(is), size(misc::get_size(*input))
	{
	}

	bool open(char *)
	{
		return true;
	}
	
	bool _readbuffer(char *buffer,int bytes)
	{
		input->read(buffer, bytes);
		return input->good();
	}
	
	int  getbytedirect(void)
	{
		return input->get();
	}
	
	bool eof(void)
	{
		return input->eof();
	}

	int  getblock(char *buffer,int size)
	{
		input->read(buffer, size);
		return (input->good()) ? size : 0;
	}

	int getsize(void)
	{
		return size;
	}
	
	int getposition(void)
	{
		return misc::get_cur_pos(*input);
	}
	
	void setposition(int pos)
	{
		misc::set_pos(*input, pos);
	}
};

class MemOutput
 : public mpegsound::Soundplayer
{
	Samples* samples;
	int cur_pos, cur_size;
	SoundInfo info;
	int count;
public:
	MemOutput()
	 : samples(0), cur_pos(0), cur_size(0), count(0)
	{
	}
	~MemOutput()
	{
	}

	void set_buffer(Samples* buf)
	{
		samples = buf;
		cur_pos = 0;
	}

	bool initialize(char *)
	{
		return true;
	}

	bool setsoundtype(int stereo,int samplesize,int speed)
	{
		info.channels = (stereo == 1) ? 2 : 1;
		info.bits_per_sample = samplesize;
		info.samplerate = speed;
		return true;
	}

	bool putblock(void *buffer,int size)
	{
		if (samples == 0)
			return false;
		char* ptr = static_cast<char*>(buffer);
		samples->resize(cur_pos + size);
		std::copy(ptr, ptr + size, samples->begin() + cur_pos);
		cur_pos += size;
		count += size;
		return true;
	}

	const SoundInfo& get_info()
	{
		return info;
	}
	int get_count() { return count; }
};

class Mp3Source : public AudioSource
{
public:
	StreamInput input;
	MemOutput output;
	mpegsound::Mpegtoraw mp3dec;

	Mp3Source(std::istream* i)
	 : input(i), mp3dec(&input, &output)
	{
		mp3dec.initialize();
		mp3dec.run(-1);
	}

	SoundInfo info()
	{
		return output.get_info();
	}

	bool read(Samples& smp)
	{
		output.set_buffer(&smp);
		if (! mp3dec.run(2)) {
			return false;
		}
		return mp3dec.getcurrentframe() != mp3dec.gettotalframe();
	}
};

Mp3Decoder::Mp3Decoder()
 : rs(0), src(0)
{
}

Mp3Decoder::~Mp3Decoder()
{
	delete src;
	delete rs;
}


bool Mp3Decoder::init(res::res_stream* is)
{
	delete src;
	delete rs;
	rs = is;
	src = new Mp3Source(rs);
	return true;
}

AudioSourcePtr Mp3Decoder::get()
{
	if (src) {
		AudioSource* as = src;
		src = 0;
		return AudioSourcePtr(as);
	} else {
		return AudioSourcePtr(new Mp3Source(rs));
	}
}


}
}
}

extern "C" {
DLL_EXPORT void* create_snd_mp3(void* m)
{
	return new reaper::os::snd::Mp3Decoder();
}
}

		



#ifdef REAPER_EMBED_PLUGINS
namespace {
struct RegisterPlugin {
	RegisterPlugin() {
		reaper::misc::preloaded::add_plugin_creator("snd_mp3", (reaper::misc::VoidCreator1)create_snd_mp3);
	}
} foo;
}
#endif


