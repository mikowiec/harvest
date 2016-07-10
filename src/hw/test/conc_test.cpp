
/*
 * $Author: pstrand $
 * $Date: 2003/01/06 12:42:05 $
 * $Log: conc_test.cpp,v $
 * Revision 1.3  2003/01/06 12:42:05  pstrand
 * namespace hw -> namespace os
 *
 * Revision 1.2  2000/11/01 00:44:12  peter
 * win32
 *
 * Revision 1.1  2000/10/12 15:47:29  peter
 * tester...
 *
 */

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <functional>
#include <algorithm>
#include <list>

#include "conc_types.h"
#include "concurrent.h"
#include "errno.h"

using namespace reaper::os::concurrent;

class PO : public unary_function<void, int> {
public:
	int operator()() {
		cout << "One at a time, please!" << endl;
		return 0;
	}
};

class Run : public runnable {
	Mutex mx;
	Semaphore sem;
	ProtectedObj<PO>& p;
public:
	Run(Mutex m, Semaphore s, ProtectedObj<PO>& po) : mx(m), sem(s), p(po) { }
	virtual void run() {
		mx.lock();
		cout << "Hello world" << endl;
		mx.unlock();
		sem.wait();
		p();
		cout << "Bye" << endl;
	}
	virtual ~Run() { }
};

int main() {
	Mutex m;
	Semaphore s;
	ProtectedObj<PO> p = PO();
	Run r(m, s, p);
	Thread t(&r);
	m.lock();
	cout << "lock&start" << endl;
	t.start();
	msleep(1000);
	cout << "unlock" << endl;
	m.unlock();
	msleep(1000);
	s.signal();
	p();
	msleep(1000);
	t.stop();
	return 0;
}

