#ifndef REAPER_MISC_IOSTREAM_HELPER_H
#define REAPER_MISC_IOSTREAM_HELPER_H

#include <string>
#include <iostream>
#include "misc/parse.h"

namespace reaper
{
namespace misc
{

inline
long get_cur_pos(std::istream& is)
{
	return is.rdbuf()->pubseekoff(0, std::ios::cur);
}

inline
void set_pos(std::istream& is, long pos)
{
	is.rdbuf()->pubseekoff(pos, std::ios::beg);
}

inline
long get_size(std::istream& is)
{
	long org_pos = get_cur_pos(is);
	long pos = is.rdbuf()->pubseekoff(0, std::ios::end);
	set_pos(is, org_pos);
	return pos;
}

template<class T>
void send(std::ostream& os, const std::string& s, const T& t)
{
	os << s << ": " << t << "\r\n" << std::flush;
}

template<class T>
void send(std::ostream& os, const T& t)
{
	os << t << "\r\n" << std::flush;
}

struct crlf_foo { };
struct spaces_foo { };
namespace {
crlf_foo crlf;
spaces_foo spaces;
}

inline
std::istream& operator>>(std::istream& is, const crlf_foo&)
{
	skip_crlf(is);
	return is;
}

inline
std::istream& operator>>(std::istream& is, const spaces_foo&)
{
	skip_spaces(is);
	return is;
}



}
}

#endif


