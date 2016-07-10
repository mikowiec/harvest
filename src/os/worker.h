
/* $Id: worker.h,v 1.2 2003/01/06 12:42:12 pstrand Exp $ */

#ifndef REAPER_HW_WORKER_H
#define REAPER_HW_WORKER_H

#include "misc/stlhelper.h"


namespace reaper {
namespace os {
namespace worker {

struct Job
{
	virtual ~Job() { }
	virtual bool operator()() = 0;
	virtual void done() { }
};

class Worker
{
public:
	virtual ~Worker();
	virtual void spawn_job(Job*) = 0;
	virtual void add_job(Job*, bool delete_when_done = false) = 0;
	virtual void shutdown() = 0;
};

Worker* worker();

}
}
}

#endif

