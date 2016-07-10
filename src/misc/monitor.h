
#ifndef REAPER_MISC_MONITOR_H
#define REAPER_MISC_MONITOR_H

#include <deque>
#include <algorithm>
#include "misc/sequence.h"
#include "misc/stlhelper.h"
#include "misc/command.h"

namespace reaper {
namespace misc {

template<class T>
class MPing {
	T val;
public:
	MPing(const T& v) : val(v) { }
	void operator()(Command<1, T>& cmd) {
		cmd(val);
	}
};


template<class T>
class Monitored
{
	typedef Command<1, T> Cmd;
	T val;
	std::deque<Cmd> listeners;
	Monitored(const Monitored<T>&);
public:
	Monitored(const T& init) : val(init) { }
	~Monitored() {
//		for_each(seq(listeners), std::mem_fun_ref(&Cmd::del));
	}
	void onchange(const Cmd& cmd) {
		listeners.push_back(cmd);
	}
	Monitored<T>& operator=(const T& v)
	{
		if (val != v) {
			val = v;
			for_each(seq(listeners), MPing<T>(val));
		}
		return *this;
	}
//	operator T() const { return val; }
	operator const T&() const { return val; }
};

class Switch
{
	bool status;
	typedef Command<0> Cmd;
	Cmd cmd;
	Switch(const Switch&);
public:
	Switch() : status(false) { }
	Switch(bool b) : status(b) { }
	void set(Cmd c) {
		cmd = c;
	}
	void on() {
		cmd();
		status = true;
	}
	void off() {
		status = false;
	}
	Switch& operator=(bool sw)
	{
		sw ? on() : off();
		return *this;
	}
	operator bool() const
	{
		return status;
	}
	// FIXME
	operator const bool&() const
	{
		return status;
	}
};

}
}

#endif



