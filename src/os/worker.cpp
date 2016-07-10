
#include "os/compat.h"

#include <functional>
#include <queue>
#include <map>

#include "os/concurrent.h"
#include "os/abstime.h"
#include "os/debug.h"
#include "os/worker.h"
#include "misc/sequence.h"
#include "misc/free.h"

namespace reaper {
namespace os {
namespace worker {

namespace cc = concurrent;

using misc::seq;
using misc::for_each;
using std::pair;
using std::queue;

class Child : public cc::Runnable
{
	Job* job;
	cc::Thread thread;
public:
	Child(Job* j) : job(j), thread(this)
	{
		thread.start();
//		printf("start\n");
	}
	~Child()
	{
		thread.stop(false);
	}
	void run()
	{
		(*job)();
		delete job;
	}
};

class Slave : public Worker
{
	struct SlaveJob
	 : public concurrent::Runnable
	{
		concurrent::Mutex jobs_mtx;

		typedef queue<pair<Job*,bool> > jobs_t;
		jobs_t jobs;
		cc::Semaphore quit_req;
		void run()
		{
			while (true) {
				pair<Job*,bool> p;
				{ concurrent::ScopeLock sc(jobs_mtx);
//				  printf("child looping, size: %d\n", jobs.size());
				  if (jobs.empty()) {
				  	if (quit_req.try_wait()) {
//				  		printf("child dying\n");
				  		return;
				  	}
				  	time::msleep(10);
				  	continue;
				  }
				  p = jobs.front();
				  jobs.pop();
				}
				if ((*p.first)()) {
//					printf("again\n");
					concurrent::ScopeLock sc(jobs_mtx);
					jobs.push(p);
				} else {
//					printf("alas, the end is here...\n");
					if (p.second)
						delete p.first;
					else
						p.first->done();
				}
				time::msleep(10);
			}
		}
	};
	SlaveJob sj;

	cc::Thread thread;
	std::deque<Child*> children;

public:
	Slave() : thread(&sj) {
		thread.start();
	}
	virtual ~Slave()
	{
		shutdown();
	}
	void spawn_job(Job* job)
	{
//		printf("spawn\n");
		children.push_back(new Child(job));
	}
	void add_job(Job* job, bool dwd)
	{
//		printf("add\n");
		concurrent::ScopeLock sc(sj.jobs_mtx);
		sj.jobs.push(std::make_pair(job, dwd));
	}
	void shutdown()
	{
//		printf("jobs left: %d, children %d\n", sj.jobs.size(), children.size());
		sj.quit_req.signal();
		thread.stop(true);
		for_each(seq(children), misc::delete_it);
	}
};

Worker::~Worker()
{ }

Worker* worker()
{
	static Slave slave;
	return &slave;
}


}
}
}


