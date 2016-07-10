

#include "os/compat.h"

#include "os/debug.h"
#include "os/abstime.h"

#include "game/scheduler.h"

namespace reaper {
namespace game {


namespace {
	reaper::debug::DebugOutput derr("scheduler");
}

template<typename I>
struct JobBox {
	int seq;
	Job* job;
	I delta;
	I next;
	I last_run;
	JobBox(int s, Job* j, I d, I now)
	 : seq(s), job(j), delta(d), next(now), last_run(now)
	{ }
};


template<typename I>
bool job_less<I>::operator()(JobBox<I>* j1, JobBox<I>* j2) const {
{
		if (j1->next > j2->next)
			return true;
		if (j1->next == j2->next)
			return j1->seq > j2->seq;
		return false;
	}
};

void Scheduler::add_time(Job* j, os::time::RelTime dt)
{
	time_q.push(new JobBox<os::time::RelTime>(seq++, j, dt, now));
}

void Scheduler::add_frame(Job* j, int frame)
{
	frame_q.push(new JobBox<int>(seq++, j, frame, now));
}

void Scheduler::clear()
{
	while (!frame_q.empty()) {
		delete frame_q.top()->job;
		delete frame_q.top();
		frame_q.pop();
	}
	while (!time_q.empty()) {
		delete time_q.top()->job;
		delete time_q.top();
		time_q.pop();
	}
}

void Scheduler::tick_frame()
{
	frame++;
}
void Scheduler::tick_time(os::time::RelTime r)
{
	now += r;
}

int Scheduler::get_frames()
{
	return frame;
}

os::time::RelTime Scheduler::get_time()
{
	return now;
}


template<typename I>
bool Scheduler::run_queue(typename Q<I>::t& q, I lim)
{
	while (!q.empty() && q.top()->next <= lim) {
		JobBox<I>* jb = q.top();
		q.pop();
		bool more = jb->job->work(*this, now - jb->last_run);
		jb->last_run = now;
		if (more) {
			jb->next += jb->delta;
			q.push(jb);
		} else {
			delete jb->job;
			delete jb;
		}
	}
	return !q.empty();
}

template bool Scheduler::run_queue<int>(Q<int>::t&, int);

void Scheduler::run()
{
	bool b1, b2;
	do {
		b1 = run_queue(frame_q, frame);
		b2 = run_queue(time_q, now);
	} while (b1 || b2);
}

	
}
}


