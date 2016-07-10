
/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:12 $
 * $Log: conc_pthread.cpp,v $
 * Revision 1.2  2003/01/06 12:42:12  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.17  2002/09/29 12:35:52  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.16  2002/02/20 23:06:38  peter
 * ntverksfixar, eventhantering, snd_simple fix, mm..
 *
 * Revision 1.15  2002/02/20 20:38:47  peter
 * sunos
 *
 * Revision 1.14  2002/01/17 04:58:41  peter
 * ljud-plugin-mm fix..
 *
 * Revision 1.13  2001/11/18 16:12:28  peter
 * hmm
 *
 * Revision 1.12  2001/04/26 03:08:55  peter
 * *** empty log message ***
 *
 * Revision 1.11  2001/04/17 17:42:20  peter
 * *** empty log message ***
 *
 * Revision 1.10  2001/04/15 15:27:50  peter
 * mest småfixar..
 *
 * Revision 1.9  2001/03/26 10:45:10  peter
 * dödade lite minnesläckor..
 *
 * Revision 1.8  2001/03/25 10:50:28  peter
 * låsning
 *
 * Revision 1.7  2001/03/21 11:21:30  peter
 * namn...
 *
 * Revision 1.6  2001/01/27 00:57:53  peter
 * namespace städ...
 *
 * Revision 1.5  2001/01/07 13:05:24  peter
 * *** empty log message ***
 *
 *
 */

#include "os/compat.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>

#include "os/concurrent.h"

namespace reaper
{
namespace os
{
namespace concurrent
{

Runnable::~Runnable()
{ }

Thread::Thread(Runnable* r) : run(r) { }

void* start_thread(void* t) {
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	(static_cast< Thread* >(t))->run->run();
	return 0;
}

void Thread::start()
{
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&id, &attr, start_thread, this);
}

void Thread::stop(bool wait_for_exit)
{
	if (wait_for_exit)
		pthread_join(id, 0);
	else
		pthread_cancel(id);
}

Mutex::Mutex() {
	mutex = new pthread_mutex_t;
	pthread_mutex_init(mutex, 0);
}

Mutex::~Mutex() {
	pthread_mutex_destroy(mutex);
	delete mutex;
}

void Mutex::lock() {
	pthread_mutex_lock(mutex);
}

bool Mutex::try_lock() {
	if (pthread_mutex_trylock(mutex) == EBUSY)
		return false;
	else
		return true;
}

void Mutex::unlock() {
	pthread_mutex_unlock(mutex);
}

Semaphore::Semaphore(int v) {
	sem = new sem_t;;
	sem_init(sem, 0, v);
}

Semaphore::~Semaphore() {
	sem_destroy(sem);
	delete sem;
}

void Semaphore::wait() {
	sem_wait(sem);
}

bool Semaphore::try_wait() {
	int r = sem_trywait(sem);
	if (r != 0)
		return false;
	else
		return true;
}

void Semaphore::signal() {
	sem_post(sem);
}

}

}
}

