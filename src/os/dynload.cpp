
#include "os/compat.h"

#include "os/dynload.h"
#include "os/debug.h"
#include "os/exceptions.h"
#include "os/osinfo.h"
#include "res/res.h"
#include "os/stat.h"

#ifdef WIN32

#include "os/windows.h"
#include "os/win32_errors.h"

#else

#include <dlfcn.h>

#endif

namespace reaper {
namespace os {
namespace dynload {

namespace { debug::DebugOutput derr("os::dynload", 5); }

#ifdef WIN32

const int RTLD_GLOBAL = 0;
const int RTLD_LAZY = 0;


const std::string dlerror()
{
	return win32_strerror();
}

HINSTANCE dlopen(const char* fn, int flags)
{
	return LoadLibrary(fn);
}

symptr dlsym(HINSTANCE h, const char* sym)
{
	return (symptr)GetProcAddress(h, sym);
}

int dlclose(HINSTANCE h)
{
	return ! FreeLibrary(h);
}

const std::string dl_ext = ".dll";

#else

const std::string dl_ext = ".so";

#endif

void error(const std::string& a, const std::string& b)
{
	throw hw_error(a + ": " + b + ' ' + dlerror());
}


DynLoader::DynLoader(const std::string& name)
 : dlname(name), handle(0)
{
	std::string path = res::find_plugin(dlname, dl_ext);
	derr << "loading: " << name << " from " << path << "\n";
	if (!(handle = dlopen(path.c_str(), RTLD_GLOBAL | RTLD_LAZY)))
		error("Error loading", name);
}

DynLoader::~DynLoader()
{
	derr << "closing " << dlname << '\n';
	if (dlclose(handle) != 0) {
		error("Error unloading", dlname);
	}
}

symptr DynLoader::load(const std::string& symbol)
{
	symptr s = (symptr)dlsym(handle, symbol.c_str());
	if (s == 0)
		error("Error locating symbol", symbol);
	return s;
}

}
}
}

