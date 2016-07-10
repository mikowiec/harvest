/* $Id: profile.cpp,v 1.2 2003/01/06 12:42:08 pstrand Exp $ */

#include "os/compat.h"
#include "os/debug.h"
#include <limits>

#include <iomanip>
#include <iostream>

#include "time.h"
#include <sstream>

#include "misc/font.h"
#include "gfx/misc.h"
#include "misc/profile.h"
#include "os/gl.h"

namespace reaper {
namespace misc {
namespace time {

using os::time::TimeSpan;

Profiler::Profiler(const std::string& n)
 : name(n), begin(0,0), total(0,0), count(0), td(0)
{ }

void Profiler::reset()
{
	begin = total = TimeSpan();
	count = 0;
}

void Profiler::start()
{
	begin = os::time::get_time();
}

long Profiler::stop()
{
	TimeSpan diff = os::time::get_time() - begin;
	total += diff;
	td = diff.approx();
	count++;
	return td;
}

int Profiler::get_count() const
{
	return count;
}

long Profiler::get_last() const
{
	return td;
}

double Profiler::get_avg() const
{
	return (count > 0) ? (total.approx() / count) : 0 ;
}

using std::string;

class Foo { } foo;

std::ostream& operator<<(std::ostream& os, const Foo&) {
	os << std::right << std::fixed << std::setprecision(0) << std::setw(6);
	return os;
}

string Profiler::mkreport(const string& desc, const string& vdesc, double val) const
{
	std::ostringstream ss;
	
	ss << (name.size() != 0 ? name + ":" : "")
	   << (desc.size() != 0 ? desc + ": " : "")
	   << " Average for " << foo << count << " sample"
	   << ((count != 1) ? "s: " : ": ")
	   << foo << get_avg() << " us";
	if (count != 1)
		ss << "  " << vdesc << ": " << foo << val << " us";
	ss << '\n';
	return ss.str();
}

string Profiler::report(const string& desc) const
{
	return mkreport(desc, "total", total.approx());
}

string Profiler::report_last(const string& desc) const
{
	return mkreport(desc, "last", get_last());
}

string Profiler::report_last_short(const string& desc) const
{
	std::ostringstream ss;
	ss << (name.size() != 0 ? name + ":" : "")
	   << (desc.size() != 0 ? desc + ": " : "")
	   << "avg: " << foo << get_avg();
	if (count != 1)
		ss << " last: " << foo << get_last();
	ss << '\n';
	return ss.str();
}

void Profiler::int_report(const std::string& desc, meth_ptr m) const
{
	static debug::DebugOutput derr("profiler");
	derr << (this->*m)(desc);
}

void Profiler::int_report(const string& desc) const
{
	int_report(desc, &Profiler::report);
}

void Profiler::int_report_last(const string& desc) const
{
	int_report(desc, &Profiler::report_last);
}

Profile::Profile(Profiler& pp)
 : pref(pp)
{
	pref.start();
}

Profile::Profile(const std::string& l)
 : label(l), p(l), pref(p)
{
	pref.start();
}

Profile::~Profile()
{
	pref.stop();
	if (!label.empty()) {
		debug::DebugOutput derr(label, 5);
		derr << ": " << pref.get_last() << " us\n";
	}
}


Profiler2::Profiler2(const std::string& s, float xx, float yy, float tl, float h, int len)
 : Profiler(s), x(xx), y(yy), textlen(tl), height(h), history(len, 0.0), ins(0)
{}

void Profiler2::draw_txt()
{
	using namespace reaper::gfx::font;
	glPutStr(x, y, report_last(), Medium, 0.008, height*.95);
}

void Profiler2::draw_txt_short()
{
	using namespace reaper::gfx::font;
	glPutStr(x, y, report_last_short(), Medium, 0.008, height*.95);
}


using reaper::gfx::misc::meter;

void Profiler2::draw_last(float frame_length)
{
	meter(x+textlen, y, 1-textlen, height/2, 1, get_last() / frame_length);
}

void Profiler2::draw_avg(float frame_length)
{
	meter(x+textlen, y+height/2, 1-textlen, height/2, 1, get_avg() / frame_length);
}

void Profiler2::draw_history(float frame_length)
{
	history[ins++] = get_last() / frame_length;
	ins %= history.size();

	glBegin(GL_POINTS);
	for (int i = 0; i < history.size(); ++i) {
		float yp = history[(ins+i) % history.size()];
		float xp = ((1-textlen)*i) / history.size();
		glVertex2f(x+textlen+xp, y + height*yp);
	}
	glEnd();
}


}
}
}
