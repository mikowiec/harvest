#ifndef REAPER_MISC_PARSE_H
#define REAPER_MISC_PARSE_H

#include <iosfwd>

#include <vector>
#include <list>
#include <string>

#include <algorithm>
#include <functional>

#include "main/types.h"
#include "main/enums.h"

namespace reaper
{
namespace misc
{


typedef const std::string& cstrr;

typedef std::string::iterator str_i;
typedef std::string::const_iterator str_ci;
typedef std::string::size_type str_s;

typedef std::vector<std::string> strvec;
typedef strvec::iterator strvec_i;
typedef strvec::const_iterator strvec_ci;

typedef std::list<std::string> strlst;
typedef strlst::iterator strlst_i;
typedef strlst::const_iterator strlst_ci;

void chomp(char *s);
void chomp(std::string &s);

long stol(const std::string& s);
int stoi(const std::string& s);
float stof(const std::string& s);
std::string ltos(long i, int n = 0, char fill = ' ');
std::string ftos(float f);

CompanyID str2company(cstrr str);
void str2point(cstrr str, Point& id);

template<class E> std::binder1st<std::equal_to<E> > equals(const E& e) {
	return std::bind1st(std::equal_to<E>(), e);
}

template<class Out>
int split(cstrr s, Out iter, int n = 0, char c = ' ')
{
	int i = 0;
	--n;
	if (s.size() == 0)
		return i;
	str_ci p = s.begin();
	str_ci e = s.end();
	str_ci p2;
	p = std::find_if(p, e, std::not1(equals(c)));
	while (p != e && i != n) {
		p2 = std::find_if(p, e, equals(c));
		*iter++ = std::string(p, p2);
		p = std::find_if(p2, e, std::not1(equals(c)));
		i++;
	}
	if (p != e) {
		*iter++ = std::string(p, e);
		++i;
	}
	return i;
}

typedef std::pair<std::string, std::string> stringpair;
inline
stringpair split(const std::string& s, char delim = ' ')
{
	stringpair res;
	std::string::size_type i = s.find(delim);
	if (i < s.size()-1) {
		res.first = s.substr(0, i);
		res.second = s.substr(i+1, s.size()-i-1);
	} else {
		res.first = s;
	}
	return res;
}

/// Parses a linebreak. ie: "\r", "\r\n" or "\n"
std::istream& skip_crlf(std::istream& is);

/// Parses zero or more blank (' ' or '\t') characters)
std::istream& skip_spaces(std::istream& is);

/// Read (and throw away) one of the given characters.
std::istream& anyof(std::istream& is, const std::string& s);

/// Read until one of the given characters.
std::istream& until(std::istream& is, std::string& s, const std::string& stop);
std::istream& until(std::istream& is, char stop); 

/// remove whitespace from the beginning and end of a string
std::string& trim(std::string& s);

inline
std::istream& skipline(std::istream& is) {
	std::string junk;
	until(is, junk, "\r\n");
	skip_crlf(is);
	return is;
}

template<class T>
struct ConfigLine
{
	std::string var;
	T val;
	bool valid;
};

template<class T>
ConfigLine<T> mk_config_line(const std::string& v, const T& t)
{
	ConfigLine<T> cl;
	cl.var = v;
	cl.val = t;
	cl.valid = true;
	return cl;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const ConfigLine<T>& cl)
{
	os << cl.var << ": " << cl.val << '\n';
	return os;
}

class Dumper
{
	std::ostream& os;
public:
	Dumper(std::ostream& o) : os(o) { }
	template<class T>
	Dumper& operator()(const std::string& v, const T& t)
	{
		os << mk_config_line(v, t);
		return *this;
	}
};

inline
Dumper dumper(std::ostream& os)
{
	return Dumper(os);
}

bool config_label(std::istream& is, std::string& var);

template<class T>
std::istream& operator>>(std::istream& is, ConfigLine<T>& cl)
{
	config_label(is, cl.var);
	is >> cl.val;
	cl.valid = is.good();
	skip_spaces(is);
	skip_crlf(is);
	return is;
}

/** Parse a config (assignment) line. format:
 *  var = val
 *  var: val
 */
bool config_line(std::istream& is, std::string& var, std::string& val);

inline
bool elem(char c, std::string s) {
	return s.find(c) != std::string::npos;
}

/** Parse many config lines.
 *  Possibly delimited by empty lines or comments
 */
template<class Cont>
bool config_lines(std::istream& is, Cont& cont, bool stop_at_nl = false)
{
	bool prelude = true;
	std::string sec;
	while (! is.eof()) {
		std::string var;
		if (elem(is.peek(), "#; \t\r\n")) {
			std::getline(is, var);
			chomp(var);
			if (prelude || !stop_at_nl || var.size() > 0)
				continue;
			else
				break;
		}
		if (is.peek() == '[') {
			is.get();
			std::getline(is, sec, ']');
			if (elem(is.peek(), " \t\r\n") && !sec.empty())
				sec += '_';
			else
				sec = "";
			continue;
		}
		std::string val;
		if (!config_line(is, var, val)) {
			continue;
		}
		if (! var.empty() && !val.empty() && !isdigit(var[0])) {
			cont[sec + var] = val;
			prelude = false;
		}
	}
	return true;
}

/// Compare two strings while ignoring case.
bool strequal(const std::string& s1, const std::string& s2);


/* used to get a table name from a path
 */
std::string basename_from_path(const std::string& name);
}
}


#endif


/*
 * $Author: pstrand $
 * $Date: 2002/10/02 13:28:04 $
 * $Log: parse.h,v $
 * Revision 1.31  2002/10/02 13:28:04  pstrand
 * netsync..
 *
 * Revision 1.30  2002/09/13 07:45:56  pstrand
 * gcc-varningar
 *
 * Revision 1.29  2002/05/04 11:12:37  pstrand
 * *** empty log message ***
 *
 * Revision 1.28  2002/03/24 17:37:17  pstrand
 * ltos fix
 *
 * Revision 1.27  2002/03/19 20:42:34  pstrand
 * *** empty log message ***
 *
 * Revision 1.26  2002/02/11 12:00:58  peter
 * no message
 *
 * Revision 1.25  2002/01/26 11:24:16  peter
 * resourceimprovements, and minor fixing..
 *
 * Revision 1.24  2001/12/11 18:11:28  peter
 * *** empty log message ***
 *
 * Revision 1.23  2001/12/09 20:29:13  peter
 * matrix io & statesave
 *
 * Revision 1.22  2001/12/09 17:31:36  peter
 * fix
 *
 * Revision 1.21  2001/12/08 23:23:54  peter
 * 'spara/fixar/mm'
 *
 * Revision 1.20  2001/12/02 18:03:31  peter
 * duh
 *
 * Revision 1.19  2001/12/02 17:40:47  peter
 * objmgrmeck
 *
 * Revision 1.18  2001/11/21 00:53:51  peter
 * fix cr/nl...
 *
 * Revision 1.17  2001/08/27 12:55:26  peter
 * objektmeck...
 *
 * Revision 1.16  2001/08/06 12:16:32  peter
 * MegaMerge (se strandy_test-grenen för diffar...)
 *
 * Revision 1.15.4.2  2001/08/05 14:01:28  peter
 * objektmeck...
 *
 * Revision 1.15.4.1  2001/08/03 13:44:07  peter
 * pragma once obsolete...
 *
 * Revision 1.15  2001/07/15 21:43:31  peter
 * småfixar
 *
 * Revision 1.14  2001/07/06 01:47:27  macke
 * Refptrfix/headerfilsstäd/objekt-skapande/mm
 *
 * Revision 1.13  2001/05/06 22:19:22  peter
 * *** empty log message ***
 *
 * Revision 1.12  2001/05/04 18:15:50  niklas
 * la till två funktioner, str2company och str2point
 *
 */

