/* $Id: snd_simple.cpp,v 1.4 2003/05/05 08:53:14 pstrand Exp $ */

#include "os/compat.h"

#include <string>
#include <deque>

#include "os/snd.h"
#include "os/snd/snd_simple.h"
#include "os/snd/snd_subsystem.h"
#include "misc/smartptr.h"
#include "misc/sequence.h"
#include "os/reltime.h"
#include "os/abstime.h"
#include "os/interfaces.h"

#include "os/worker.h"
#include "misc/plugin.h"


namespace reaper
{
namespace os
{
namespace snd
{
namespace simple
{

/*
class Job : public worker::Job
{
public:
	virtual bool run() = 0;
};
*/


class SimpleEffect
 : public Effect
{
	struct PlayJob : public worker::Job
	{
		AudioSourcePtr src;
		Player player;
		bool imm, playing, exit, more, loop;
		SoundInfo info;

		Samples smp;
		bool done_write;
		volatile bool wait_exit;

		PlayJob(AudioSourcePtr s, bool i)
		 : src(s), imm(i), playing(false), exit(false), more(false), loop(false),
		   done_write(false), wait_exit(true)
		{
//			printf("create 1 %x\n", this);
			info = src->info();
			more = src->read(smp);
			player.prepare(info, smp.size(), more); // first chunk size determines player block size
		}
		PlayJob(SoundInfo i, Samples s)
		 : src(0), imm(false), playing(false), exit(false), more(false), loop(false),
		   info(i), smp(s), done_write(false), wait_exit(true)
		{
//			printf("create 2 %x\n", this);
			player.prepare(info, s.size(), false);
		}
		~PlayJob()
		{
			stop();
			wait_end();
//			printf("dead %x\n", this);
		}
		bool operator()()
		{
//			printf("run %x %d %d %d\n", this, playing, exit, wait_exit);
			if (playing) {
//				printf("2 play! %d\n", more);
				player.play(smp);
				if (more)
					more = src->read(smp);
				else if (!loop)
					playing = false;
			} else {
//				printf("2 no data\n");
			}
			return !exit;
		}
		void done() {
//			printf("done %x\n", this);
			delete src;
			player.release();
			wait_exit = false;
//			printf("released\n");
		}

		void play() {
//			printf("play %x %d\n", this, exit);
			playing = true;
		}
		void stop() { 
//			printf("stop %x %d\n", this, wait_exit);
			playing = false;
			exit = true;
		}
		void wait_end()
		{
//			printf("wait end %x %d %d %d\n", this, playing, exit, wait_exit);
			while (wait_exit) {
				os::time::msleep(5);
			}
		}
		void looping(bool l)
		{
			loop = l;
		}
		
	};
	misc::SmartPtr<PlayJob> job;
public:
	SimpleEffect(AudioSourcePtr sd, bool imm = false)
	 : job(new PlayJob(sd, imm))
	{
	}
	SimpleEffect(misc::SmartPtr<PlayJob> j)
	 : job(j)
	{ }
	~SimpleEffect()
	{
//		printf("simpleeffect die %x\n", job.get());
	}
	worker::Job* get_job() {
		return job.get();
	}

	virtual void play()
	{
		job->play();
	}

	virtual void stop()
	{
		job->stop();
	}
	

	virtual void set_volume(float vol)
	{
		job->player.set_volume(vol);
	}
	virtual void set_position(const Point& pos)
	{
	}

	virtual void set_direction(const Vector& vec)
	{
	}

	virtual void set_velocity(const Vector& vel)
	{
	}
	virtual void set_pitch(float p)
	{
	}
	virtual void looping(bool l)
	{
		job->looping(l);
	}
	bool is_done()
	{
		return true;
	}
	virtual EffectPtr clone() const
	{
		return EffectPtr(new SimpleEffect(job));
	}
};



class Subsystem
: public snd::Subsystem
{
//	typedef std::deque<misc::SmartPtr<Job> > Jobs;
//	Jobs jobs;
	os::time::RelTime last;
	ifs::Snd* main;
	float vol;
public:
	Subsystem(ifs::Snd* s) : last(0), main(s) { }
	~Subsystem()
	{ }
	bool init() { return true; }
	EffectPtr prepare(AudioSourcePtr);
	SoundPtr prepare_streaming(AudioSourcePtr);
	void set_volume(float vol);
	void set_listener(const Point& pos, const Vector& dir, const Vector&) { }
	void do_stuff();
};


EffectPtr Subsystem::prepare(AudioSourcePtr sd)
{
	SimpleEffect* e = new SimpleEffect(sd, true);
	e->set_volume(vol);
	main->add_job(e->get_job());
	return EffectPtr(e);
}

SoundPtr Subsystem::prepare_streaming(AudioSourcePtr sd)
{
	SimpleEffect* e = new SimpleEffect(sd);
	e->set_volume(vol);
	main->add_job(e->get_job());
	return SoundPtr(e);
}

void Subsystem::set_volume(float v)
{
	vol = v;
}

void Subsystem::do_stuff()
{
/*
	Jobs::iterator c, e = jobs.end();
	for (c = jobs.begin(); c != e; ++c) {
		(*c)->run();
	}
*/
}

}
}
}
}


extern "C" {
DLL_EXPORT void* create_snd_simple(reaper::os::ifs::Snd* s)
{
	return new reaper::os::snd::simple::Subsystem(s);
}
}



#ifdef REAPER_EMBED_PLUGINS
namespace {
struct RegisterPlugin {
	RegisterPlugin() {
		reaper::misc::preloaded::add_plugin_creator("snd_simple", (reaper::misc::VoidCreator1)create_snd_simple);
	}
} foo;
}
#endif
