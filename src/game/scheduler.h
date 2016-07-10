/*
 * Scheduler for the main loop, makes things a bit 
 * modular than before (with the hard-coded loop).
 * But also longer and less explicit, perhaps the
 * whole thing is silly?
 * We'll see what happens after game/helpers.cpp has
 * been cleaned up properly.
 *
 * An option to specify order of execution is
 * probably needed.
 * They are now scheduled in the order they were added.
 *
 */

#ifndef REAPER_GAME_SCHEDULER_H
#define REAPER_GAME_SCHEDULER_H


#include "os/reltime.h"

#include "misc/command.h"

#include <vector>
#include <queue>

namespace reaper {
namespace game {	


class Scheduler;

using os::time::RelTime;

struct Job {
	virtual bool work(Scheduler&, os::time::RelTime dt) = 0;
	virtual ~Job() { }
};


class MiscCmd : public Job
{
	misc::Ping cmd;
public:
	MiscCmd(misc::Ping c) : cmd(c) { }
	bool work(Scheduler&, os::time::RelTime) {
		cmd();
		return true;
	}
};

class MiscSwitchCmd : public Job
{
	misc::Ping cmd;
	const bool& cont;
public:
	MiscSwitchCmd(misc::Ping c, const bool& ct) : cmd(c), cont(ct) { }
	bool work(Scheduler&, os::time::RelTime) {
		if (cont)
			cmd();
		return true;
	}
};


template<typename I> struct JobBox;

template<typename I>
struct job_less {
	bool operator()(JobBox<I>* j1, JobBox<I>* j2) const;
};

template<typename I>
struct Q {
	typedef std::priority_queue< JobBox<I>*, std::vector<JobBox<I>*>, job_less<I> > t;
};

class Scheduler {
public:
	Scheduler() : seq(0), now(0), frame(0) { }

	void add_time(Job* j, os::time::RelTime dt);
	void add_frame(Job* j, int frame);

	void clear();
	void tick_frame();
	void tick_time(os::time::RelTime r);
	int get_frames();
	os::time::RelTime get_time();
		
	template<class I>
	bool run_queue(typename Q<I>::t& q, I lim);
	void run();

private:
	int seq;
	os::time::RelTime now;
	int frame;

	Q<int>::t frame_q;
	Q<os::time::RelTime>::t time_q;
};



}
}

#endif

