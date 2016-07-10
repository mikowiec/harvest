

#include "os/compat.h"

#include <string>
#include <sstream>

#include "res/resource.h"
#include "res/config.h"
#include "res/res.h"
#include "os/debug.h"
#include "misc/sequence.h"
#include "misc/parse.h"
#include "main/types_io.h"

#include "misc/lua.h"

namespace reaper
{
namespace res
{

using std::string;
using namespace misc;

namespace {
	debug::DebugOutput dlog("config", 5);
	const std::string empty_str;
}


bool ConfVal::operator==(bool b) const
{
	return (b == bool(*this));
}

bool ConfVal::operator==(int i) const
{
	return i == int(*this);
}

bool ConfVal::operator==(long l) const
{
	return l == long(*this);
}

bool ConfVal::operator==(float f) const
{
	return f == float(*this);
}

bool ConfVal::operator==(const string& s) const
{
	return s == string(*this);
}

bool ConfVal::operator==(const char* p) const
{
	return string(p) == string(*this);
}

ConfVal::operator bool() const
{
	return (val == "1") || (strequal(val.substr(0, 4), "true"));
}


ConfVal::operator int() const
{
	if (val.empty())
		return 0;
	std::istringstream istr(val);
	int ival = 0;
	istr >> ival;
	return ival;
}

ConfVal::operator long() const
{
	if (val.empty())
		return 0;
	std::istringstream istr(val);
	long lval = 0;
	istr >> lval;
	return lval;
}


ConfVal::operator float() const
{
	if (val.empty())
		return 0;
	std::istringstream fstr(val);
	float fval = 0;
	fstr >> fval;
	return fval;
}

ConfVal::operator const string() const
{
	return val;
}

const std::string ConfVal::str() const
{
	return val;
}

ConfVal::operator Point() const
{
	return read<Point>(val);
}

ConfVal::operator Vector() const
{
	return read<Vector>(val);
}

ConfVal::operator Matrix() const
{
	return read<Matrix>(val);
}

MConfVal& MConfVal::operator=(bool b)
{
	mval = (b) ? "true" : "false";
	return *this;
}

MConfVal& MConfVal::operator=(int i)
{
	mval = ltos(i);
	return *this;
}

MConfVal& MConfVal::operator=(long l)
{
	mval = ltos(l);
	return *this;
}

MConfVal& MConfVal::operator=(float f)
{
	mval = ftos(f);
	return *this;
}

MConfVal& MConfVal::operator=(const string& s)
{
	mval = s;
	return *this;
}


std::string vec_str(std::vector<std::string>& v)
{
	int i = 0, n = v.size();
	std::string s("[");
	while (i < n) {
		s += v[i++];
		if (i != n)
			s += ", ";
	}
	s += "]";
	return s;
}

class ConfigNode : public NodeConfig<ConfigEnv> {

	Ident mk_path(IdentRef id) {
		if (id.find('/') == string::npos)
			return "config/" + id;
		else
			return id;
	}
	void read_env(ConfigEnv& env, IdentRef id, bool print_err = true) {
		std::string path = mk_path(id);
		try {
			misc::lua::Lua lua;
			misc::lua::Table tbl = load_table(lua, path);

			tbl.begin_iter();
			while (tbl.more()) {
				misc::lua::Val val = tbl.get_val();
				if (val.type() == misc::lua::type::Table) {
					std::vector<std::string> vec;
					read_push_vector(lua, val, vec);
					env[tbl.get_key()] = vec_str(vec);
				} else {
					env[tbl.get_key()] = std::string(val);
				}
			}
			tbl.end_iter();
			return;
		} catch (misc::lua::lua_load_error) {
			dlog << "load error " << path << '\n';
		} catch (misc::lua::lua_type_error) {
			dlog << "type error " << path << '\n';
		} catch (resource_not_found) {
			if (print_err)
				dlog << "not found " << path << '\n';
		}

		res_stream fr(File, path);
		if (fr.good()) {
			env.read(fr);
		}
	}
public:
	typedef tp<ConfigEnv>::ptr Ptr;

	ConfigNode() { }

	Ptr create(IdentRef id) {
		Ptr p = Ptr(new ConfigEnv());
		read_env(*p, id+".defaults", false);
		read_env(*p, id);
		dlog << "Reading config: " << id << '\n';
		return p;
	}

	void save(IdentRef id, const ConfigEnv& env) {
		res_out_stream out(File, mk_path(id), true);
		out << env;
	}
};

struct InitLoader {
	InitLoader() {
		define_node<ConfigEnv>(new ConfigNode());
	}
	~InitLoader() { }
} stat_init;

ConfigEnv::ConfigEnv() { }

ConfigEnv::ConfigEnv(const string& m)
 : module(m)
{
	merge(resource<ConfigEnv>(m));
}

ConfigEnv::ConfigEnv(const ConfigEnv& env)
{
	merge(env);
}

ConfigEnv::ConfigEnv(std::istream& is, bool stop_at_empty_line)
{
	config_lines(is, env, stop_at_empty_line);
}

ConfigEnv& ConfigEnv::operator=(const ConfigEnv& o)
{
	merge(o);
	return *this;
}

void ConfigEnv::read(std::istream& is, bool stop_at_empty_line)
{
	config_lines(is, env, stop_at_empty_line);
}

bool ConfigEnv::defined(const string& var) const
{
	return env.find(var) != env.end();
}


const ConfVal ConfigEnv::operator[](const string& var) const
{
	Env::const_iterator i = env.find(var);
	return (i != env.end()) ? ConfVal(i->second) : ConfVal(empty_str);
}

MConfVal ConfigEnv::operator[](const string& var)
{
	return MConfVal(env[var]);
}

struct write {
	std::ostream& os;
	write(std::ostream& o) : os(o) { }
	int operator()(std::pair<const string, string>& p) const {
		os << p.first << ": " << p.second << '\n';
		return 42;
	}
};

void ConfigEnv::merge(const ConfigEnv& other)
{
	for (Env::const_iterator i = other.begin(); i != other.end(); ++i) {
		env.insert(*i);
	}
}

std::ostream& operator<<(std::ostream& os, const ConfigEnv& env)
{
	ConfigEnv::iterator c = env.begin(), e = env.end();
	bool in_subsec = false;

	while (c != e) {
		string pfx = c->first.substr(0, c->first.find('_'));
		ConfigEnv::iterator n = c;
		while (n != e && pfx == n->first.substr(0, pfx.size()))
			++n;
		if (std::distance(c, n) > 3) {
			os << "\n[" << pfx << "]\n";
			for (; c != n; ++c)
				os << c->first.substr(pfx.size()+1) << ": " << c->second << '\n';
			os << '\n';
			in_subsec = true;
		} else {
			if (in_subsec)
				os << "\n[]\n";
			for (; c != n; ++c)
				os << c->first << ": " << c->second << '\n';
		}
	}
	return os;
}

}

}


