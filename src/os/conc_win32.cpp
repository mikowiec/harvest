
/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:12 $
 * $Log: conc_win32.cpp,v $
 * Revision 1.2  2003/01/06 12:42:12  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.1  2002/12/29 16:58:14  pstrand
 * moved from hw/
 *
 * Revision 1.12  2002/09/29 12:35:52  pstrand
 * inverting dependencies: phys
 *
 * Revision 1.11  2002/01/17 04:58:42  peter
 * ljud-plugin-mm fix..
 *
 * Revision 1.10  2001/10/16 21:10:46  peter
 * video & win32 fixar
 *
 * Revision 1.9  2001/04/29 13:47:56  peter
 * windows...
 *
 * Revision 1.8  2001/04/15 15:27:51  peter
 * mest småfixar..
 *
 * Revision 1.7  2001/03/25 12:34:06  peter
 * no message
 *
 * Revision 1.6  2001/03/21 11:21:31  peter
 * namn...
 *
 * Revision 1.5  2000/11/02 01:12:59  peter
 * win32.. vc.. ugh..
 *
 * Revision 1.4  2000/11/01 01:14:30  peter
 * mer win32...
 *
 * Revision 1.3  2000/11/01 00:44:07  peter
 * win32
 *
 * Revision 1.2  2000/10/08 16:12:06  peter
 * namespace & fullscreen
 *
 * Revision 1.1  2000/09/28 10:40:30  peter
 * lite experiment....
 *
 */

#include <iostream>
#include <cstdio>
#include <functional>
#include <algorithm>

#include "conc_types.h"
#include "concurrent.h"
#include "errno.h"

#include "os/windows.h"

namespace reaper
{
namespace os
{
namespace concurrent
{

Runnable::~Runnable()
{ }

void* start_thread(void* v)
{
	Thread* t = static_cast<Thread*>(v);
	t->run->run();
	t->death.signal();
	ExitThread(0);
	return 0;
}

DWORD WINAPI fptr(void* v) {
	start_thread(v);
	return 0;
}

Thread::Thread(Runnable* r) : run(r) {
	DWORD lp;
	id = CreateThread(0, 0, fptr, this, CREATE_SUSPENDED, &lp);
}

void Thread::start() {
	ResumeThread(id);
}

void Thread::stop(bool wait) {	// FIXME wait for thread....
	if (wait)
		death.wait();
	else
		TerminateThread(id, 0);
}


Mutex::Mutex() {
	mutex = CreateMutex(0, 0, 0);
}

Mutex::~Mutex() {
}

void Mutex::lock() {
	WaitForSingleObject(mutex, INFINITE);
}

bool Mutex::try_lock() {
	if (WaitForSingleObject(mutex, 0) == WAIT_TIMEOUT)
		return false;
	else
		return true;
}

void Mutex::unlock() {
	ReleaseMutex(mutex);
}

Semaphore::Semaphore(int v) {
	sem = CreateSemaphore(0, v, 10, 0);	    // FIXME 10 <> max
}

Semaphore::~Semaphore() {

}

void Semaphore::wait() {
	WaitForSingleObject(sem, INFINITE);
}

bool Semaphore::try_wait() {
	if (WaitForSingleObject(sem, 0) == WAIT_TIMEOUT)
		return false;
	else
		return true;
}

void Semaphore::signal() {
	ReleaseSemaphore(sem, 1, 0);
}

}
}
}

