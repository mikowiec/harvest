

#ifndef REAPER_OBJECT_MKINFO_H
#define REAPER_OBJECT_MKINFO_H

#include "main/enums.h"
#include <map>
#include <string>
#include <iosfwd>
#include "res/config.h"

namespace reaper {

namespace object {

class MkInfo
{
public:
	static res::ConfigEnv empty;

	std::string name;
	Matrix mtx;
	CompanyID cid;
	int id;
	res::ConfigEnv info;
	res::ConfigEnv cdata;

	MkInfo();
	MkInfo(const std::string& n, const Matrix& m,
	       int i, CompanyID ci = Nature,
	       const res::ConfigEnv& inf = empty,
	       const res::ConfigEnv& c = empty);
};

std::ostream& operator<<(std::ostream&, const MkInfo&);
std::istream& operator>>(std::istream&, MkInfo&);

}
}

#endif
