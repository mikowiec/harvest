#ifndef REAPER_HW_CONCURRENT_H
#define REAPER_HW_CONCURRENT_H

#include "os/conc_types.h"

///
namespace reaper
{
///
namespace os
{
/// Primitives for concurrent programming.
namespace concurrent
{

using os::lowlevel::thread_t;
using os::lowlevel::mutex_t;
using os::lowlevel::sema_t;

/// Mutex lock.
class Mutex {
	mutex_t mutex;
public:
	Mutex();
	~Mutex();

	/// Lock mutex (blocking).
	void lock();

	/// Try to lock mutex, returns false if it would block.
	bool try_lock();

	/// Unlock mutex.
	void unlock();
};

class Lock {
	Mutex mtx;
public:
	Lock() { mtx.lock(); }
	~Lock() { mtx.unlock(); }
};

class ScopeLock
{
	Mutex& mtx;
public:
	ScopeLock(Mutex& m) : mtx(m) { mtx.lock(); }
	~ScopeLock() { mtx.unlock(); }
};

/// Semaphore.

class Semaphore {
	sema_t sem;
public:
	Semaphore(int v = 0);
	~Semaphore();

	/// Wait (blocking).
	void wait();

	/// Try to wait on semaphore, returns false if it would block.
	bool try_wait();

	/// Signal semaphore.
	void signal();
};

/// Inherit this to be runnable.
class Runnable {
public:
	virtual ~Runnable();

	virtual void run() = 0;
};

void* start_thread(void*);

/// Minimal thread abstraction.
class Thread {
	thread_t id;
	Semaphore death;
	Runnable* run;
	friend void* start_thread(void*);
public:
	Thread(Runnable* u);
	void start();
	void stop(bool wait_for_exit = false);
};


}
}
}

#endif


/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:12 $
 * $Log: concurrent.h,v $
 * Revision 1.2  2003/01/06 12:42:12  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.18  2002/09/29 12:35:52  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.17  2002/09/21 17:36:10  pstrand
 * configure
 *
 * Revision 1.16  2002/01/17 04:58:42  peter
 * ljud-plugin-mm fix..
 *
 * Revision 1.15  2001/10/16 21:10:46  peter
 * video & win32 fixar
 *
 * Revision 1.14  2001/08/06 12:16:19  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.13.4.1  2001/08/03 13:43:56  peter
 * pragma once obsolete...
 *
 * Revision 1.13  2001/07/06 01:47:18  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.12  2001/04/27 01:05:32  peter
 * *** empty log message ***
 *
 * Revision 1.11  2001/04/16 22:14:19  peter
 * *** empty log message ***
 *
 * Revision 1.10  2001/04/15 15:27:51  peter
 * mest småfixar..
 *
 * Revision 1.9  2001/03/25 10:50:28  peter
 * låsning
 *
 * Revision 1.8  2001/03/21 11:21:31  peter
 * namn...
 *
 * Revision 1.7  2001/01/27 00:57:53  peter
 * namespace städ...
 *
 * Revision 1.6  2001/01/07 13:05:25  peter
 * *** empty log message ***
 *
 *
 */

