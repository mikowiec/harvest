#ifndef REAPER_RES_CONFIG_H
#define REAPER_RES_CONFIG_H

#include "main/exceptions.h"
#include "main/types.h"

#include <map>
#include <string>
#include <iosfwd>

namespace reaper
{
namespace res
{

void init_config_node();

class ConfVal
{
	const std::string& val;
public:
	ConfVal(const std::string& s) : val(s) { }
	operator bool() const;
	operator int() const;
	operator long() const;
	operator float() const;
	operator Matrix() const;
	operator Vector() const;
	operator Point() const;
	operator const std::string() const;

	const std::string str() const;

	bool operator==(bool b) const;
	bool operator==(int i) const;
	bool operator==(long i) const;
	bool operator==(float f) const;
	bool operator==(const std::string& s) const;
	bool operator==(const char* p) const;
	template<class T>
	bool operator!=(const T& t) const {
		return !(*this == t);
	}
};

class MConfVal : public ConfVal
{
	std::string& mval;
public:
	MConfVal(std::string& s) : ConfVal(s), mval(s) { }
	MConfVal& operator=(bool b);
	MConfVal& operator=(int i);
	MConfVal& operator=(long l);
	MConfVal& operator=(float f);
	MConfVal& operator=(const std::string& s);
};


class ConfigEnv
{
	const std::string module;
public:
	typedef std::map<std::string, std::string> Env;
	typedef Env::const_iterator iterator;
	typedef std::pair<std::string, std::string> value_type;

	ConfigEnv();
	ConfigEnv(const std::string& module);
	ConfigEnv(std::istream &is, bool stop_at_empty_line = false);
	ConfigEnv(const ConfigEnv&);
	~ConfigEnv() { }
	ConfigEnv& operator=(const ConfigEnv&);
	void read(std::istream&, bool stop_at_empty_line = false);
	bool defined(const std::string&) const;
	const ConfVal operator[](const std::string& var) const;
	MConfVal operator[](const std::string& var);
	iterator begin() const { return env.begin(); }
	iterator end() const { return env.end(); }
	int empty() const { return env.empty(); }
	void merge(const ConfigEnv& other);
private:
	Env env;
};

std::ostream& operator<<(std::ostream& os, const ConfigEnv& env);

template<class T>
struct fix_str { typedef T t; };

template<> struct fix_str<char*> { typedef std::string t; };
template<> struct fix_str<const char*> { typedef std::string t; };

template<class T>
const typename fix_str<T>::t withdefault(const ConfigEnv& conf, const std::string& var, T def)
{
	if (conf.defined(var))
		return conf[var];
	else
		return def;
}

class Dumper
{
	ConfigEnv& env;
public:
	Dumper(ConfigEnv& e) : env(e) { }
	template<class T>
	Dumper& operator()(const std::string& var, const T& t) {
		env[var] = t;
		return *this;
	}
};

inline
Dumper dumper(ConfigEnv& env) { return Dumper(env); }


void init_config_loader();

}
}

#endif


