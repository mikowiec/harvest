
#include "os/compat.h"

#include <cctype>
#include <string>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <map>

#include "misc/parse.h"

namespace reaper
{
namespace misc
{

using std::string;
using std::istream;

void chomp(char *s) {
	if (!s)
		return;
	int i = strlen(s) - 1;
	while (i >= 0 && (s[i] == '\n' || s[i] == '\r'))
		s[i--] = '\0';
}

void chomp(string &s) {
	int i = s.size() - 1;
	while (i >= 0 && (s[i] == '\n' || s[i] == '\r'))
		i--;
	s.resize(i+1);
}

long stol(cstrr s)
{
	if (s.size())
		return atol(s.c_str());
	else
		return 0;
}

int stoi(cstrr s)
{
	if (s.size())
		return atoi(s.c_str());
	else
		return 0;
}

float stof(cstrr s)
{
	std::istringstream is(s);
	float f = 0;
	is >> f;
	return f;
}

string ltos(long l, int n, char f)
{
	std::ostringstream os;
	os.width(n);
	os.fill(f);
	os << l;
	return os.str();
}

string ftos(float f)
{
	std::ostringstream os;
	os << f;
	return os.str();
}

CompanyID str2company(cstrr str)
{
	if (strequal(str,"slick"))
		return Slick;
	else if (strequal(str,"horny"))
		return Horny;
	else if (strequal(str,"shell")) 
		return Shell;
	else if (strequal(str,"punkrocker"))
		return Punkrocker;
	else if (strequal(str,"village"))
		return Village;
	else if (strequal(str,"projectile"))
		return Projectile;
	else
		return Nature;
}

void str2point(cstrr str, Point& p)
{
	string numeric = "1234567890-.";
	str_s pos1, pos2=0;
	float f[3];

	for(int i=0; i<3; i++){
		pos1 = str.find_first_of(numeric,pos2);
		pos2 = str.find_first_not_of(numeric, pos1);
		f[i] = stof(str.substr(pos1,pos2-1));		
	}

	p.x = f[0];
	p.y = f[1];
	p.z = f[2];
}

istream& skip_crlf(istream& is)
{
	if (is.peek() == '\r') {
		is.get();
		if (is.peek() == '\n') {
			is.get();
		}
	} else if (is.peek() == '\n')  {
		is.get();
	} else
		is.setstate(std::ios::failbit);
	return is;
}

bool isws(char c)
{
	return c == ' ' || c == '\t';
}

istream& skip_spaces(istream& is)
{
	while (isws(is.peek())) {
		is.get();
	}
	return is;
}


istream& anyof(istream& is, const string& s)
{
	if (elem(is.peek(), s))
		is.get();
	else
		is.setstate(std::ios::failbit);
	return is;
}

istream& until(istream& is, string& s, const string& stop)
{
	while (!is.eof() && !elem(is.peek(), stop) && is.good()) {
		s += is.get();
	}
	return is;
}

istream& until(istream& is, char stop)
{
	while (!is.eof() && is.peek() != stop && is.good())
		is.get();
	return is;
}

string& trim(string& s)
{
	string::size_type b, e;
	b = s.find_first_not_of(" \t\r\n");
	e = s.find_last_not_of(" \t\r\n");
	if (b == string::npos)
		b = 0;
	if (e == string::npos)
		e = s.size() - 1;
	string s2(s, b, e - b + 1); // FIXME temporary workaround for gcc3
	s = s2;
	return s;
}

bool config_label(istream& is, string& var)
{
	until(is, var, ":=\r\n");
	anyof(is, ":=");
	trim(var);
	return is.good();
}

bool config_line(istream& is, string& var, string& val)
{
	string junk;
	if (config_label(is, var)) {
		until(is, val, "#;\r\n");
		std::getline(is, junk);
		trim(val);
		return true;
	} else {
		is.clear();
		std::getline(is, junk);
		return false;
	}
}


namespace {
bool icasecmp(char c1, char c2)
{
	return tolower(c1) == tolower(c2);
}
}

bool strequal(const string& s1, const string& s2)
{
	if (s1.size() != s2.size())
		return false;
	return std::equal(s1.begin(), s1.end(), s2.begin(), icasecmp);
}


std::string basename_from_path(const std::string& name)
{
	/* extract filename from name */
	int last_slash_at = name.rfind('/');
	int fn_idx = (last_slash_at == std::string::npos) ? 0 : last_slash_at+1;
	int dot_at = name.find('.', fn_idx);
	return name.substr(fn_idx, dot_at-fn_idx);
}
		

}
}

/*
std::map<std::string, int*> refptrs;

void add_refptr(const char* name, int* p)
{
	refptrs[name] = p;
}

void del_refptr(const char* name)
{
	refptrs.erase(std::string(name));
}

void report_refs()
{
	std::map<std::string, int*>::iterator i = refptrs.begin();
	for (; i != refptrs.end(); ++i) {
		reaper::debug::std_debug << "Refptr: " << i->first << " count: " << *i->second << '\n';
	}
}

*/
