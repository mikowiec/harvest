#ifndef REAPER_HW_DYNLINK_H
#define REAPER_HW_DYNLINK_H

#include <string>

#include "os/debug.h"
#include "misc/base.h"

#ifdef WIN32
#include "os/windows.h"
#endif

namespace reaper {
namespace os {
namespace dynload {

//#ifdef WIN32
//typedef void* (__stdcall *symptr)(void*);
//#else
typedef void* (*symptr)(void*);
//#endif

class DynLoader : misc::NoCopy
{
	std::string dlname;
#ifdef WIN32
	HINSTANCE handle;
#else
	void* handle;
#endif

public:
	DynLoader(const std::string& name);
	~DynLoader();

	symptr load(const std::string& symbol);

	template<typename Symb>
	void load(const std::string& symbol, Symb& sym)
	{
		sym = (Symb)(load(symbol));
	}
};


}
}
}

#endif
